import json
import networkx as nx
import re
import pprint
import os

# Operation_Funcs = {}

# Deep first search
def find_callee_funcs(G, adj, depth, Sets, OPs):
    if len(adj) == 0:
        return

    for callee_nid in adj:
        try:
            print("#"*depth, end="")

            if G.nodes[callee_nid]:
                print(G.nodes[callee_nid]['label'])
                # add the function which will be called under this Operation
                func_name = ''.join(re.findall("\"{(.*?)}\"", G.nodes[callee_nid]['label']))
                func_name = func_name.split("|")[0]
                tmp = '"{%s}"' % (func_name)
                if tmp in OPs:
                    continue

                # llvm.ctlz.i32
                # llvm.dbg.declare
                # llvm.memset.p0i8.i32
                # llvm.memcpy.p0i8.p0i8.i32
                if func_name.startswith("llvm."):
                    print(func_name)
                    continue

                if func_name in Sets:
                    continue

                Sets.add(func_name)

            find_callee_funcs(G, G[callee_nid], depth+1, Sets, OPs)

        except RecursionError:
            print("RecursionError")


def decorate_operation_name(operation_entry_funcs):
    docorated_operation_names = []
    for entry_func in operation_entry_funcs:
        decorated_func = '"{' + entry_func + '}"'
        docorated_operation_names.append(decorated_func)
    return docorated_operation_names


def search(G, OPs):
    """
    search reachable functions for an operation
    @OPs: operation entry functions
    """
    Operation_Funcs = {}
    pprint.pprint(OPs)
    for nid, attrs in G.nodes.data():
        # print(nid, attrs, G[nid])
        # Find Operation Entry Node
        if attrs.get('shape') == 'record' and attrs.get('label') in OPs:
            Operation_Name = ''.join(re.findall("\"{(.*?)}\"", attrs.get('label')))
            Operation_Name = Operation_Name.split("|")[0]
            print("[+] Load Operation Entry Function: " + attrs.get('label'))
            Sets = set()
            # Find the child node of the current node
            find_callee_funcs(G, G[nid], 1, Sets, OPs)
            # print("These Functions will be called under This Operation")
            Operation_Funcs[Operation_Name] = list(Sets)
            # print(json.dumps(Lists,ensure_ascii=False,indent=4))
            del Sets
    return Operation_Funcs


def get_operation_subfuncs(entry_info, callgraph):
    """
    get reachable functions of each operation
    entry_funcs_file: operation entry functions list
    callgraph: callgraph of the application, networkx.classes.multidigraph.MultiDiGraph type
    """
    OperationNames = list(entry_info.keys())
    # callgraph = nx.drawing.nx_pydot.read_dot(callgraph_file)
    OperationNames = decorate_operation_name(OperationNames)
    Operation_Funcs = search(callgraph, OperationNames)
    return Operation_Funcs


def get_node_id_by_function_name(callgraph, function_name):
    for node_id, attrs in callgraph.nodes.data():
        temp = ''.join(re.findall("\"{(.*?)}\"", attrs.get('label')))
        if function_name == temp:
            return node_id


def callgraph_edge_pruning(callgraph, pruned_edges):
    """
    pruning spurious indirect call edges in callgraph
    callgraph: callgraph object
    pruned_edges: pruned edges from address-taken or type-based pruning, format:
        {
            "caller": [pruned_targets],
        }
    """
    for edge in callgraph.edges.data():
        caller_id = edge[0]
        callee_id = edge[1]
        attr = edge[2]
        if attr["color"] == "red":
            caller_name = callgraph.nodes[caller_id]["label"].strip()[2:-2]
            callee_name = callgraph.nodes[callee_id]["label"].strip()[2:-2]
            print("{}->{}".format(caller_name, callee_name))
            if caller_name in pruned_edges.keys():
                if callee_name in pruned_edges[caller_name]:
                    # this edge in the pruned_edges dictionary, should be removed in callgraph
                    callgraph.remove_edge(caller_id, callee_id)
                    print("pruned")
    return callgraph


def solve_unsolved_indirect_calls(callgraph, ander_json, type_json, address_taken_json):
    for caller_func, callsites in ander_json.items():
        for each_callsite in callsites:
            ## 1) if the callsite doesn't have targets. If not, it is an unsolved indirect call
            ## Note that, if the callsite is calling an assembly instruction, the targets should be null
            if each_callsite["targets"] != None:
                continue

            caller_id = get_node_id_by_function_name(callgraph, caller_func)

            ## 2) type-based analysis
            if caller_func not in type_json.keys():
                # all the callsites in call_func are calling inline assembly
                continue

            for callsite_type in type_json[caller_func]:
                if (each_callsite["location"] == callsite_type["location"]) and callsite_type["targets"]:
                    ## we use the callsite's location to match
                    for candicate_callee in callsite_type["targets"]:
                        ## 3) check if the target function is address taken
                        if candicate_callee not in address_taken_json:
                            continue

                        ## 4) if so, add the target to the callgraph
                        callee_id = get_node_id_by_function_name(callgraph, candicate_callee)
                        callgraph.add_edge(caller_id, callee_id, color="red")

    return callgraph


if __name__ == "__main__":
    env_dist = os.environ
    OI_DIR = env_dist["OI_DIR"]
    print("OI_DIR: ", OI_DIR)

    entry_funcs_file = OI_DIR + "/stm32f407/pinlock/Decode/SW4STM32/STM32F4-DISCO/" + "build9/" + "Entry_functions.json"
    callgraph_file = OI_DIR +  "/stm32f407/pinlock/Decode/SW4STM32/STM32F4-DISCO/" + "build9/" + "callgraph.dot"
    # callgraph_file = OI_DIR + "/stm32f407/pinlock/Decode/SW4STM32/STM32F4-DISCO/" + "callgraph.dot"
    Operation_Funcs = get_operation_subfuncs(entry_funcs_file, callgraph_file)
    pprint.pprint(Operation_Funcs)