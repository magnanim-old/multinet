\name{Network management: updates}
\alias{Network management: updates}
\alias{add.layers.ml}
\alias{add.actors.ml}
\alias{add.nodes.ml}
\alias{add.edges.ml}

\alias{delete.layers.ml}
\alias{delete.actors.ml}
\alias{delete.nodes.ml}
\alias{delete.edges.ml}
\title{
Manipulation of multilayer networks
}
\description{
Functions to add or remove components of a multilayer network.

The functions \code{add.nodes.ml} and \code{delete.nodes.ml} add/remove the input actors to/from the input layers, but do not add/remove the actors from the multilayer network.
}
\usage{
add.layers.ml(mlnetwork, layers, directed=FALSE)
add.actors.ml(mlnetwork, actors)
add.nodes.ml(mlnetwork, nodes)
add.edges.ml(mlnetwork, edges)

delete.layers.ml(mlnetwork, layers)
delete.actors.ml(mlnetwork, actors)
delete.nodes.ml(mlnetwork, nodes)
delete.edges.ml(mlnetwork, edges)
}
\arguments{
\item{mlnetwork}{A multilayer network.}
\item{layers}{An array of names of layers.}
\item{actors}{An array of names of actors.}
\item{directed}{Determines if the layer(s) is (are) directed or undirected. If multiple layers are specified, directed should be either a single value or an array with as many values as the number of layers.}
\item{nodes}{A dataframe of nodes to be updated. The first column specifies actor names, the second layer names.}
\item{edges}{A dataframe containing the nodes to be connected. The four columns must contain, in this order: actor1 name, layer1 name, actor2 name, layer2 name. The directionality of the edge (directed/undirected) is pre-defined depending on the layer(s).}
}
\seealso{
\link{Network management: properties}, \link{Network management: edge directionality}
}
\examples{
net <- ml.empty()
# Adding some layers
add.layers.ml(net,"l1")
add.layers.ml(net,c("l2","l3"),c(TRUE,FALSE))
layers.ml(net)
# Adding actors A1, A2, A3
add.actors.ml(net,"A1")
add.actors.ml(net,c("A2","A3"))
# Verifying that the actors have been added correctly
num.actors.ml(net)
actors.ml(net)
# Adding some nodes (actor A3 is not present in layer l3: no corresponding node there)
nodes <- data.frame(
    c("A1","A2","A3","A1","A2","A3"),
    c("l1","l1","l1","l2","l2","l2"))
add.nodes.ml(net,nodes)
nodes <- data.frame(
    c("A1","A2"),
    c("l3","l3"))
add.nodes.ml(net,nodes)
nodes.ml(net)
# We create a data frame specifying two edges:
# A2,l2 -- A3,l1
# A2,l2 -- A3,l2
edges <- data.frame(
    c("A2","A2"),
    c("l2","l2"),
    c("A3","A3"),
    c("l1","l2"))
add.edges.ml(net,edges)
edges.ml(net)

# The following deletes layer 1, ans also deletes
# all nodes from "l1" and the edge with an end-point in "l1"
delete.layers.ml(net,"l1")
# The following also deletes the nodes associated to
# "A1" in layers "l2" and "l3"
delete.actors.ml(net,"A1")
# deleting node A2,l3 and edge A2,l2 -- A3,l2
delete.nodes.ml(net,data.frame("A2","l3"))
edges <- data.frame("A2","l2","A3","l2")
delete.edges.ml(net,edges)
net
}
