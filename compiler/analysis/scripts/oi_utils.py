import json
import networkx
import pickle

def extract_name_bracket(line):
    """
    extract name in ()
    """
    idx1 = line.find('(')
    idx2 = line.find(')')
    name = line[idx1+1:idx2]
    return name


def extract_name_brace(line):
    """
    extract name in {}
    """
    idx1 = line.find('{')
    idx2 = line.find('}')
    # point_to_target_set = line[idx1+1:idx2].strip().split()
    point_to_target_set = line[idx1+1:idx2]
    return point_to_target_set


def extract_name_angle(line):
    """
    extract name in <>
    """
    idx1 = line.find('<')
    idx2 = line.find('>')
    name = line[idx1+1:idx2]
    return name


class PTATarget(object):
    def __init__(self, target_id, target_name, node_type):
        super().__init__()
        self.id = target_id
        self.name = target_name
        self.node_type = node_type

    @property
    def get_id(self):
        return self.id

    def set_id(self, new_id):
        self.id = new_id

    @property
    def get_name(self):
        return self.name

    def set_name(self, new_name):
        self.name = new_name


class Pointer(object):
    def __init__(self, pointer_name, func_name, targets_num):
        super().__init__()
        self.pointer_name = pointer_name
        self.func_name = func_name           # pointer where func exists
        self.pt_targets = set()
        self.targets_num = targets_num
        self.pointer_type = None

    @property
    def get_pointer_name(self):
        return self.pointer_name

    def set_pointer_name(self, new_pointer_name):
        self.pointer_name = new_pointer_name

    @property
    def get_func(self):
        return self.func_name

    def set_func_name(self, new_func_name):
        self.func_name = new_func_name

    @property
    def get_pt_targets(self):
        return self.pt_targets
    
    def add_pt_target(self, target):
        self.pt_targets.add(target)
    
    def rm_pt_target(self, target):
        self.pt_targets.remove(target)

    @property
    def get_pointer_type(self):
        return self.pointer_type


class SetEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, set):
            return list(obj)
        return json.JSONEncoder.default(self, obj)


def store_json_to_file(json_to_dump, filename):
    with open(filename, "w") as f:
        json.dump(json_to_dump, f, sort_keys=True, indent=4, cls=SetEncoder)


def load_json_from_file(filename):
    with open(filename, "r") as f:
        json_to_load = json.load(f)
    return json_to_load


def store_graph_to_file(graph, filename):
    networkx.drawing.nx_pydot.write_dot(graph, filename)


def load_graph_from_file(filename):
    graph = networkx.drawing.nx_pydot.read_dot(filename)
    return graph


def rbar_to_addr(rbar):
    print(hex(rbar>>5<<5))
    return rbar>>5<<5


def rasr_to_size(rasr):
    AP = rasr >> 24 & 0x7
    N = rasr >> 1 & 0x1F
    if N < 4:
        print("Minimal permitted size is 32B")
    elif 4 <= N <= 30:
        print(hex(2**(N+1)), AP)
        return 2**(N+1)
    elif N == 31:
        print(hex(4*1024*1024*1024), AP)
        return 4*1024*1024*1024
    else:
        return 0


def next_power_2(size):
    if size == 0:
        return 0
    elif 0< size <=32:
        return 32
    else:
        return 1 << (size - 1).bit_length()


def take_first(ele):
    return ele[0]


def take_second(ele):
    return ele[1]