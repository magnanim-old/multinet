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
set.directed.ml(mlnetwork, directionalities)
is.directed.ml(mlnetwork, layers1=character(0), layers2=character(0))
}
\arguments{
\item{mlnetwork}{A multilayer network.}
\item{directionalities}{A dataframe with three columns where each row contains a pair of layers (l1,l2) and 0 or 1 (indicating resp. undirected and directed edges). Directionality is automatically set for both (l1,l2) and (l2,l1).}
\item{layers1}{The layer(s) from where the edges start. If layers1 is not provided, all layers are considered.}
\item{layers2}{The layer(s) where the edges end. If an empty list of layers is passed (default), the ending layers are set as equal to those in parameter layers1.}
}
\value{
\code{is.directed.ml} returns a data frame where each row contains the name of two layers and the corresponding type of edges (directed/undirected).
}
\examples{
net <- ml.empty()
# Adding some layers, one directed and one undirected
add.layers.ml(net,c("l1","l2"),c(TRUE,FALSE))
# Setting the directionality of inter-layer edges
layers = c("l1","l2")
dir <- data.frame(layers,layers,c(0,1))
set.directed.ml(net,dir)
# retrieving all directionalities
dir <- is.directed.ml(net)
# copying directionalities to a new network
net2 <- ml.empty()
add.layers.ml(net2,c("l1","l2"))
set.directed.ml(net2,dir)
}
