\name{multinet-package}
\alias{multinet-package}
\alias{multinet}
\docType{package}
\title{
Multilayer social network analysis and mining
}
\description{
This package defines a class to store multilayer networks and functions to pre-process, analyze and mine them.

With \emph{multilayer social network} we indicate a network where nodes (V) are organized into multiple layers (L) and each node corresponds to an actor (A), where the same actor can be mapped to nodes in different layers. Formally, a multilayer social network as implemented in this package is a graph G = (V,E) where V is a subset of A x L.

In this manual, Section \emph{\link{Getting networks: IO}} describes functions to read and write multilayer networks from/to file and the file format. To quickly test some features of the library, some existing multilayer networks are also included (\emph{\link{Getting networks: predefined}}). A synthetic multilayer network can be generated usign the growing models described in \emph{\link{Getting networks: generation}}.

Updating and getting information about the basic components of a multilayer network (layers, actors, nodes and edges) can be done using the methods described in \emph{\link{Network management: properties}}, \emph{\link{Network management: updates}} and \emph{\link{Network management: edge directionality}}. \emph{\link{Navigation}} shows how to retrieve the neighbors of a node. Attribute values can also be attached to the basic components of a multilayer network (actors, layers, nodes and edges). Attribute management is described in \emph{\link{Network management: attributes}}.

Each individual layer as well as combination of layers obtained using the data pre-processing (flattening and projection) functions described in \emph{\link{Transformation}} can be analyzed as a single-layer network using the iGraph package, by converting them
as shown in \emph{\link{Conversion}}. We can also visualize the network using the method described in \emph{\link{Plotting}}, which is also based on the plotting functionality of the iGraph package.

Multilayer network analysis measures are described in \emph{\link{Measures: basic}} (for single-actor, degree-based measures), \emph{\link{Measures: distance-based}} (for measures based on geodesic distances), \emph{\link{Measures: random walks}} (for measures based on random walks) and \emph{\link{Measures: layer comparison}} (to compare different layers).

(Overlapping) communities can be extracted as described in \emph{\link{Mining: communities}}.

Most of the methods provided by this package are described in the book "Multilayer Social Networks". These methods have been proposed by many different authors: extensive references are available in the book, and in the documentation of each function we indicate the main reference we have followed for the implementation. For a few methods developed after the book was published we give specific references to the corresponding literature.
}
%\details{
%\tabular{ll}{
%Package: \tab multinet\cr
%Type: \tab Package\cr
%Version: \tab 1.0\cr
%Date: \tab 2015-10-01\cr
%License: \tab GPL\cr
%}
%}
\author{
Matteo Magnani <matteo.magnani@it.uu.se>
}
\references{
Dickison, Magnani, and Rossi, 2016. Multilayer Social Networks. Cambridge University Press. ISBN: 978-1107438750
}
\keyword{ multilayer network social analysis mining }
