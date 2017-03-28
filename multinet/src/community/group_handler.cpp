//
//  group_handler.cpp
//  group_handler
//
//  Created by Lucas Jeub on 21/11/2012.
//
// usage:
//
//  [output]=group_handler('function_handle',input)
//
//  implemented functions are 'assign', 'move', 'moverand', 'return'
//
//      assign: takes a group vector as input and uses it to initialise the "group_index"
//
//
//      move:   takes a node index and the corresponding column of the modularity matrix as
//              input
//
//              moves node to group with maximum improvement in modularity (stays in same group
//              if no improvement possible)
//
//              returns improvement if given an output argument
//
//
//      moverand:   takes a node index and the corresponding column of the modularity matrix as
//              input
//
//              moves node to random group with improvement in modularity (stays in same group
//              if no improvement possible). Chooses from possible moves uniformly at random.
//
//              returns improvement if given an output argument
//
//      moverandw:   takes a node index and the corresponding column of the modularity matrix as
//              input
//
//              moves node to random group with improvement in modularity (stays in same group
//              if no improvement possible). Chooses from possible moves with probability
//              proportional to increase in modularity.
//
//              returns improvement if given an output argument
//
//
//      return: outputs the community assignment for all nodes as a tidy group vector, that is
//              e.g. S = [1 2 1 3] rather than S = [3 1 3 2]
//
//
// Version: 2.1
// Date: Tue 29 Nov 2016 15:29:58 EST

namespace mlnet {

using namespace std;

//find possible moves

}
