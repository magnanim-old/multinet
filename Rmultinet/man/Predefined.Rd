\name{Getting networks: predefined}
\alias{Getting networks: predefined}
\alias{ml.empty}
\alias{ml.aucs}
\alias{ml.florentine}
\alias{ml.toy}
\title{
Loading predefined multilayer networks
}
\description{
Creates predefined multilayer networks from the literature.
\itemize{
\item \code{ml.empty} returns an empty multilayer network, not containing any actor, layer, node or edge.
\item \code{ml.aucs} returns the AUCS multiplex network described in \cite{Rossi and Magnani, 2015. "Towards effective visual analytics on multiplex networks". Chaos, Solitons and Fractals. Elsevier.}
\item \code{ml.florentine} returns Padgett's Florentine Families multiplex network.
\item \code{ml.toy} returns the toy network used as a running example in \cite{Dickison, Magnani and Rossi. "Multilayer Social Networks". Cambridge University Press.}
}
}
\usage{
ml.empty(name="unnamed")
ml.aucs()
ml.florentine()
ml.toy()
}
\arguments{
\item{name}{The name of the new multilayer network.}
}
\value{
All these functions return a multilayer network.
}
\references{
Rossi, Luca, and Magnani, Matteo (2015). Towards effective visual analytics on multiplex and multilayer networks. Chaos, Solitons and Fractals, 72, 68–76. (for ml.aucs())
Padgett, John F., and McLean, Paul D. (2006). Organizational Invention and Elite Transformation: The Birth of Partnership Systems in Renaissance Florence. American Journal of Sociology, 111(5), 1463–1568. (for ml.florentine())
}
\seealso{
\link{Getting networks: IO}, \link{Getting networks: generation}
}
\examples{
empty <- ml.empty("new network")
aucs <- ml.aucs()
florentine <- ml.florentine()
}
