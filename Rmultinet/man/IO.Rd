\name{Getting networks: IO}
\alias{Getting networks: IO}
\alias{read.ml}
\alias{write.ml}
\title{
Reading and writing multilayer networks from/to file
}
\description{
These functions are used to store a multilayer network to a file or load it from a file.

The format used by the package is the following:

\verb{
 -- comment lines start with two dashes (--)

 #TYPE multiplex
 -- or: "#TYPE multilayer", if there are inter-layer edges

 #ACTOR ATTRIBUTES
 AttributeName1,STRING
 AttributeName2,NUMERIC
 -- etc.

 #NODE ATTRIBUTES
 LayerName1,AttributeName1,STRING
 LayerName1,AttributeName2,NUMERIC
 LayerName2,AttributeName3,STRING
 -- etc.

 #EDGE ATTRIBUTES
 -- if type is multiplex (default), edge attributes are indicated as follows:
 LayerName1,AttributeName,STRING
 LayerName2,AttributeName,NUMERIC
 -- etc.
 -- if type is multilayer, edge attributes are indicated as follows:
 -- LayerName1,LayerName1,AttributeName,STRING
 -- LayerName1,LayerName2,AttributeName,NUMERIC
 -- etc.


 #LAYERS
 -- if type is multiplex (default), layers are indicated as follows:
 LayerName1,UNDIRECTED,AttributeValueList...
 LayerName2,DIRECTED,AttributeValueList...
 -- etc.
 -- if type is multilayer, layers are indicated as follows:
 -- LayerName1,LayerName1,UNDIRECTED,AttributeValueList...
 -- LayerName2,LayerName2,DIRECTED,AttributeValueList...
 -- LayerName1,LayerName2,DIRECTED
 -- etc. and all intra-layer directionalities should be listed before inter-layer directionalities

 #ACTORS
 ActorName1,AttributeValueList...
 ActorName2,AttributeValueList...
 -- etc.

 #NODES
 ActorName1,LayerName1,AttributeValueList...
 ActorName1,LayerName2,AttributeValueList...
 -- etc.

 #EDGES
 -- if TYPE is multiplex (default), edges are indicated as follows:
 ActorName1,ActorName2,LayerName1,AttributeValueList...
 -- etc.
 -- if TYPE is multilayer, edges are instead indicated as follows:
 -- ActorName1,LayerName1,ActorName2,LayerName2,AttributeValueList...
 -- etc.

 ----------------------------------
}

 If the #LAYERS section is empty, undirected layers are created as mentioned in the #EDGES section.

 If the #LAYER ATTRIBUTES, #ACTOR ATTRIBUTES, #NODE ATTRIBUTES or #EDGE ATTRIBUTES sections are empty, no attributes are created.

 The #LAYERS #ACTORS and #NODES sections are useful only if attributes are present, or if there are actors not present in any layer, or if there are isolated nodes, otherwise they can be omitted.

 If no section is specified, #EDGES is the default.

 Therefore, a minimalistic undirected multiplex network file would look like this:
 \verb{
 ----------------------------------
 Matteo,Luca,Facebook
 Matteo,Mark,Facebook
 Luca,Mark,Twitter
 ----------------------------------
 }
}
\usage{
read.ml(file, name="unnamed", sep=',', aligned=FALSE)
write.ml(mlnetwork, file, format="multilayer", layers=character(0),
  sep=',', merge.actors=TRUE, all.actors=FALSE)
}
\arguments{
\item{file}{The path of the file storing the multilayer network.}
\item{name}{The name of the multilayer network.}
\item{mlnetwork}{A multilayer network.}
\item{layers}{If specific layers are passed to the function, only those layers are saved to file.}
\item{format}{Either "multilayer", to use the package's internal format, or "graphml".}
\item{sep}{The character used in the file to separate text fields.}
\item{aligned}{If true, all actors are added to all layers.}
\item{merge.actors}{Whether the nodes corresponding to each single actor should be merged into a single node (true) or kept separated (false), when format="graphml" is used.}
\item{all.actors}{Whether all actors in the multilayer network should be included in the output file (true) or only those present in at least one of the input layers (false), when format="graphml" and merge.actors=TRUE are used.}
}
\value{
\code{read.ml} returns a multilayer network. \code{write.ml} does not return any value.
}
\seealso{
\link{Getting networks: predefined}, \link{Getting networks: generation}
}
\examples{
# writing a network to file...
file <- tempfile("aucs.mpx")
net <- ml.aucs()
write.ml(net,file)
# ...and reading it back into a variable
net <- read.ml(file,"AUCS")
net
# the following network has more nodes, because all
# actors are replicated to all graphs
net.aligned <- read.ml(file,"AUCS",aligned=TRUE)
net.aligned
}
