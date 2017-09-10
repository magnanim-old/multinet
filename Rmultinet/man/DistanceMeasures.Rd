\name{Measures: distance-based}
\alias{Measures: distance-based}
\alias{distance.ml}
\title{
Network analysis measures: distance based
}
\description{
This function is based on the concept of multilayer distance. This concept generalizes single-layer distance to a vector with the distance traveled on each layer (in the "multiplex" case). Therefore, non-dominated path lengths are returned instead of shortest path length, where one path length dominates another if it is not longer on all layers, and shorter on at least one. A non-dominated path length is also known as a Pareto distance. Finding all multilayer distances can be very time-consuming for large networks.
}
\usage{
distance.ml(mlnetwork,from,to=character(0),method="multiplex")
}
\arguments{
\item{mlnetwork}{A multilayer network.}
\item{from}{The actor from which the distance is computed.}
\item{to}{The actor(s) to which the distance is computed. If not specified, all actors are considered.}
\item{method}{This argument can take values "simple", "multiplex", "full". Only "multiplex" is currently implemented.}
}
\value{
A data frame with one row for each non-dominated distance, specifying the number of steps in each layer.
}
\references{
Magnani, Matteo, and Rossi, Luca (2013). Pareto Distance for Multi-layer Network Analysis. In Social Computing, Behavioral-Cultural Modeling and Prediction (Vol. 7812, pp. 249-256). Springer Berlin Heidelberg.
}
\examples{
net <- ml.aucs()
distance.ml(net,"U54","U3")
}
