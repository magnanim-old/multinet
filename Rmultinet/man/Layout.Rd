\name{Layout}
\alias{Layout}
\alias{layout.multiforce.ml}
\alias{layout.circular.ml}
\title{
Layouts
}
\description{
These functions compute xyz coordinates for each node in the network.
}
\usage{
layout.multiforce.ml(mlnetwork, w_in=1, w_inter=1, gravity=0, iterations=100)
layout.circular.ml(mlnetwork)
}
\arguments{
\item{mlnetwork}{A multilayer network.}
\item{w_in}{An array with weights for intralayer forces, or a single number if weights are the same for all layers.}
\item{w_inter}{An array with weights for interlayer forces, or a single number if weights are the same for all layers.}
\item{gravity}{An array with weights for gravity forces, or a single number if weights are the same for all layers.}
\item{iterations}{Number of iterations.}
}
\seealso{
\link{Plotting}
}
\examples{
net <- ml.florentine()
layout.multiforce.ml(net)
l <- layout.circular.ml(net)
\dontrun{
plot(net,layout=l)
}
}
