from xml.etree import ElementTree
from xml.etree.ElementTree import Element
from sys import argv

class States:
    def __init__(self):
        self.reorder = []
        self.normal_states_count = 0
        self.error_id = 0
        self.accept_states = []
        self.done_states = []
        return
        
    def load_xml(self, am: Element):
        self.accept_states = []
        self.done_states = []

        norm = []
        done = []
        err = 0
        size = 1

        for elem in am:
            if elem.tag != 'state':
                continue
            
            name = elem.get('name')
            if name is None:
                raise ValueError('expected name attrib in state element')
            id = elem.get('id')
            if id is None:
                raise ValueError('expected id attrib in state element')
            id = int(id)

            if id + 1> size: size = id + 1

            if name.endswith('done'):
                done.append(id)
                self.done_states.append([id, name])
            elif name == 'err':
                err = id
            else:
                if name.endswith('accept'):
                    self.accept_states.append([id, name])
                norm.append(id)

        self.reorder = [0] * size
        newid = 0

        for n in norm:
            self.reorder[n] = newid
            newid = newid + 1
        self.normal_states_count = newid

        for d in done:
            self.reorder[d] = newid
            newid = newid + 1

        self.reorder[err] = newid
        self.error_id = newid

        for i in range(0, len(self.accept_states)):
            self.accept_states[i][0] = self.reorder[self.accept_states[i][0]]
        for i in range(0, len(self.done_states)):
            self.done_states[i][0] = self.reorder[self.done_states[i][0]]

    def print_cinfo(self):
        for s in self.accept_states:
            name = (NAMESPACE + s[1]).upper()
            print(f'#define {name} {s[0]}')
        name = (NAMESPACE + 'FINISHSTATES').upper()
        print(f'#define {name} {self.normal_states_count}')
        for s in self.done_states:
            name = (NAMESPACE + s[1]).upper()
            print(f'#define {name} {s[0]}')
        name = (NAMESPACE + 'ERR').upper()
        print(f'#define {name} {self.error_id}')
        print()

class State:
    def __init__(self, edge_type_count, error_state):
        self.edges = [error_state] * edge_type_count
        self.still_default = [True] * edge_type_count

class EdgeIter:
    def __init__(self, code):
        self.chars = []
        self.cur = 0

        if code == '*':
            self.chars = list(x for x in range(0, 256))
            return

        for trans_range in code.split(','):
            if len(trans_range) == 3 and trans_range[1] == '-':
                for c in range(ord(trans_range[0]), ord(trans_range[2]) + 1):
                    self.chars.append(c)
            else:
                self.chars.append(ord(trans_range[0]))

    def __iter__(self):
        return self

    def __next__(self):
        if self.cur >= len(self.chars):
            raise StopIteration

        c = self.chars[self.cur]
        self.cur = self.cur + 1
        return c

def arrsep(index, end_index):
    return ',\n' if index == end_index else ', '

class Transitions:
    def __init__(self):
        self.edge_type_count = 0
        self.edge_table = [0] * 256
        self.states = []
        return

    def print_ctable(self):
        print('#if EKNUM_SPACE_EFFICIENT\n')
        print('// Edge table')
        print(f'static const uint8_t {NAMESPACE}edges[256] = {{')
        for i in range(0, 16):
            print('\t', end='')
            for j in range(0, 16):
                print(f'{self.edge_table[i * 16 + j]}', end=arrsep(j, 15))
        print('};\n')

        print('// State transition table (small)')
        print(f'static const uint8_t {NAMESPACE}trans'
              + f'[][{self.edge_type_count}] = {{')
        for i in range(0, len(self.states)):
            print('\t{ ', end='')
            for j in range (0, self.edge_type_count // 16):
                if j != 0: print('\t  ', end='')
                for k in range(0, 16):
                    to = self.states[i].edges[j * 16 + k]
                    print(f'{to}',
                          end=arrsep(k, 15))
                    if to < 10: print(' ', end='')
            if self.edge_type_count > 16: print('\t  ', end='')
            for j in range (self.edge_type_count // 16 * 16,
                            self.edge_type_count):
                to = self.states[i].edges[j]
                print(f'{to}', end='')
                if to < 10: print(' ', end='')
                if j < self.edge_type_count - 1: print(', ', end='')
                else: print(' ', end='')
            print('},')
        print('};\n')
        print('#else // EKNUM_SPACE_EFFICIENT\n')
        print('// State transition table (big)')
        print(f'static const uint8_t {NAMESPACE}trans[][256] = {{')
        for i in range(0, len(self.states)):
            for j in range (0, 16):
                if j != 0: print('\t  ', end='')
                else: print('\t{ ', end='')
                for k in range(0, 16):
                    edge = self.edge_table[j * 16 + k]
                    to = self.states[i].edges[edge]
                    print(f'{to}', end='')
                    if to < 10: print(' ', end='')

                    if k == 15:
                        if j == 15:
                            print(', ', end='')
                        else:
                            print(',\n', end='')
                    else:
                        print(', ', end='')
            print('},')
        print('};\n')
        print('#endif // EKNUM_SPACE_EFFICIENT\n')

    def load_xml(self, states: States, am: Element):
        edge_tables = set()

        # Load in edge types first
        for elem in am:
            if elem.tag != 'transition':
                continue

            elem_read = elem.find('read')
            if elem_read is None or elem_read.text is None:
                raise ValueError('invalid state transition')

            table = [False] * 256

            ntrue = 0
            for c in EdgeIter(elem_read.text):
                table[c] = True
                ntrue += 1

            if ntrue != 256:
                edge_tables.add(tuple(table))

        # Create edge table
        edge_tables = list(edge_tables)
        self.edge_table = [0] * 256
        self.edge_type_count = 1
        overlaps_table = [0] * pow(2, len(edge_tables))

        for c in range(0, 256):
            perm = 0
            for (ti, table) in enumerate(edge_tables):
                if table[c]:
                    perm += 1 << ti
            if perm and overlaps_table[perm] == 0:
                overlaps_table[perm] = self.edge_type_count
                self.edge_type_count += 1
            self.edge_table[c] = overlaps_table[perm]

        # Create states next
        self.states = []
        for _ in range(0, states.normal_states_count):
            self.states.append(State(self.edge_type_count, states.error_id))
        for elem in am:
            if elem.tag != 'transition':
                continue

            (elem_from, elem_to, elem_read) = (
                elem.find('from'),
                elem.find('to'),
                elem.find('read')
            )
            if (elem_from is None or elem_from.text is None
                or elem_to is None or elem_to.text is None
                or elem_read is None or elem_read.text is None):
                raise ValueError('invalid state transition')

            state_id = states.reorder[int(elem_from.text)]
            to_id = states.reorder[int(elem_to.text)]
            if elem_read.text == '*':
                for d in range(0, self.edge_type_count):
                    if not self.states[state_id].still_default[d]: continue
                    self.states[state_id].edges[d] = to_id
            else:
                for c in EdgeIter(elem_read.text):
                    edge_id = self.edge_table[c]
                    self.states[state_id].edges[edge_id] = to_id
                    self.states[state_id].still_default[edge_id] = False

# Make sure the user passed in a file to generate tables for
if len(argv) < 4:
    print('usage:')
    print('python3 gendfa.py [dfa.jff] "filename" "namespace"')
    exit(-1)

FILE_NAME = argv[2]
NAMESPACE = argv[3]

# Read in XML data
root = ElementTree.parse(argv[1]).getroot()
if (automaton := root.find('automaton')) is None:
    raise RuntimeError('expected automaton element in file')
else:
    root = automaton

# Parse data
states = States()
states.load_xml(root)

trans = Transitions()
trans.load_xml(states, root)

# Print out C tables
print('// Don\'t touch this file, this is auto generated by gendfa.py')
print('// Include this in function to have local variables')
print('// Also include stdint.h before this')
print(f'#ifndef _{FILE_NAME}_h_')
print(f'#define _{FILE_NAME}_h_\n')
print(f'//#include <stdint.h>\n')
trans.print_ctable()
states.print_cinfo()
print(f'#endif // _{FILE_NAME}_h_')

