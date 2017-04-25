\name{Network management: edge directionality}
\alias{Network management: edge directionality}
\alias{set.directed.ml}
\alias{is.directed.ml}
\title{
Controlling edge directionality
}
\description{
Functions to get and set the edge directionality of one or more pairs of layers (that is, the directionality of edges connecting nodes in those layers).
}
\usage{
set.directed.ml(mlnetwork, layers, directed=TRUE)
is.directed.ml(mlnetwork, layers1=character(0), layers2=character(0))
}
\arguments{
\item{mlnetwork}{A multilayer network}
\item{layers}{A vector of layer pairs (l1,l2) whose edge directionality should be updated. Each pair is represented by two consecutive entries in the vector, and the directionality is automatically set for both (l1,l2) and (l2,l1).}
\item{directed}{Determines if the layer(s) is (are) directed or undirected. If multiple layers are specified, directed should be either a single value or an array with as many values as the number of layers. As by default layers in multilayer networks are undirected, \code{set.directed.ml} is typically called to set directed layers, in which case this parameter can be omitted.}
\item{layers1}{The layer(s) from where the edges start. If layers1 is not provided, all layers are considered.}
\item{layers2}{The layer(s) where the edges end. If an empty list of layers is passed (default), the ending layers are set as equal to those in parameter layers1.}
}
\value{
\code{is.directed.ml} returns a matrix where each row contains the name of two layers and the corresponding type of edges (directed/undirected).
}
\examples{
net <- ml.empty()
# Adding some layers, one directed and one undirected
add.layers.ml(net,c("l1","l2"),c(TRUE,FALSE))
# Setting the directionality of inter-layer edges
layers = c("l1","l2")
set.directed.ml(net,layers)
# retrieving all directionalities
is.directed.ml(net)
}