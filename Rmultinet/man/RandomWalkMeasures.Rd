\name{Measures: random walks}
\alias{Measures: random walks}
\alias{occupation.ml}
\title{
Network analysis measures
}
\description{
These functions compute network analysis measures based on multilayer random walks. These measures are currently implemented for networks without inter-layer edges.
}
\usage{
occupation.ml(mlnetwork,transitions,teleportation=.2,steps=0)
}
\arguments{
\item{mlnetwork}{A multilayer network}
\item{transitions}{An LxL matrix (with L=number of layers in the network) specifying the probability to switch from one layer to the other. The diagonal indicates the probability to remain in the same layer.}
\item{teleportation}{The probability to restart the walk from a random node.}
\item{steps}{The number of times the walker tries to move from one node to the other. The number of actual moves can be smaller, if the walker gets to nodes from where it can no longer proceed until the next teleportation. If steps=0, than a default value is chosen, currently set to 100 times the number of edges in the network.}
}
\value{
For every actor, the number of times the walker passes from it coming from another actor.
}
\references{
De Domenico, Manlio, Albert Sole-Ribalta, Elisa Omodei, Sergio Gomez, and Alex Arenas. 2015. "Centrality in Interconnected Multilayer Networks." Nature Communications, 6868.
}
\examples{
net <- ml.aucs()
transitions <- matrix(c(
.40,.15,.15,.15,.15,
.15,.40,.15,.15,.15,
.15,.15,.40,.15,.15,
.15,.15,.15,.40,.15,
.15,.15,.15,.15,.40),5,5,byrow=TRUE)
occupation <- occupation.ml(net,transitions,.2,10000)
}