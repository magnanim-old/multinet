loadModule("multinet",TRUE)

# Functions loading some famous small datasets
# More datasets can be downloaded from the book webpage: multilayer.it.uu.se

ml.aucs <- function() {
	read.ml(system.file("extdata", "aucs.mpx", package="multinet"),"AUCS")
}

ml.toy <- function() {
	read.ml(system.file("extdata", "book.mpx", package="multinet"),"toy")
}

ml.florentine <- function() {
	read.ml(system.file("extdata", "florentine_families.mpx", package="multinet"),"Florentine families")
}

# Casting of (a portion of) a multilayer network into an igraph (multi)graph. This is done by creating an intermediate graphml file and loading it as an igraph file

as.igraph.Rcpp_RMLNetwork <- function (x, layers=NULL, merge.actors=TRUE, all.actors=FALSE, ...) {
    if (is.null(layers)) {
        layers <- layers.ml(x)
    }
    temp_f <- tempfile()
    write.ml(x, temp_f, format = "graphml", layers, ",", merge.actors, all.actors)
    g <- read.graph(temp_f, format = "graphml")
    unlink(temp_f)
    g
}

# (Rudimentary) plotting functions. plot uses igraph to plot each layer, either into a single multigraph or sliced into one graph for each layer. plot3d uses rgl to plot each layer on a plane in a 3-dimensional space. Layouts for the spatial plot are computed using the multiforce.ml function.

# 2d plot
plot.Rcpp_RMLNetwork <- function(x, method="multi", layers=NULL, layer.colors=NULL, grid=NULL, layout=NULL, layout.par=list(), layout.independent=TRUE, communities=NULL,...) {
    if (is.null(layers)) {
        layers <- layers.ml(x)
    }
    
    temp_g <- as.igraph(x, layers)
    
    if (is.null(layer.colors)) {
        layer.colors <- rainbow(length(layers))
    }
    edge_colors <- rep(1,length(E(temp_g)))
    col_index <- 1
    for (layer in layers) {
        edge_colors[E(temp_g)$e_type==layer] <- layer.colors[col_index]
        col_index <- col_index+1
    }
    
    if (is.null(layout)) {
        layout <- layout.fruchterman.reingold
        l <- do.call(layout,c(list(temp_g),layout.par))
    }
    else {
        l <- layout
    }
       
    if (method=="multi") {
 	    if (is.null(communities)) {
    		igraph.groups = list();
    	}
    	else {
        	igraph.groups = get.groups.ml(communities);
    	}
        plot(temp_g,edge.color=edge_colors,layout=l,mark.groups=igraph.groups,...)
    }
    if (method=="slice") {
        if (is.null(grid)) {
            grid = c(1,num.layers.ml(x))
        }
        par(mfrow=grid)
        col_index <- 1
        for (layer in layers) {
            temp_g <- as.igraph(x,layer)
            vertex_color <- rep("SkyBlue2",length(V(temp_g)))
            vertex_color[get.vertex.attribute(temp_g,paste("layer:",layer))=="F"] <- "white"
            if (layout.independent) {
                l <- do.call(layout,c(list(temp_g),layout.par))
            }
            if (is.null(communities)) {
    			igraph.groups = list();
    		}
    		else {
	        	igraph.groups = get.groups.ml(communities[communities[,2]==layer,]);
    		}
        
           	par(mar=c(0,0,0,0))
            plot(temp_g,vertex.label=V(temp_g)$id,vertex.color=vertex_color,edge.color=layer.colors[col_index],layout=l,mark.groups=igraph.groups,...)
            col_index <- col_index+1
            box()
        }
        par(mfrow=c(1,1))
    }
}


# 3d plot

draw.arrow <- function(x1,y1,x2,y2,z,arrow.length) {
    dx <- x2-x1
    dy <- y2-y1
    edge.length <- sqrt(dx*dx+dy*dy)
    dispx <- arrow.length*dx/edge.length
    dispy <- arrow.length*dy/edge.length
    
    fcx <- x2 - dispx
    fcy <- y2 - dispy
    
    f1x <- c(x2, fcx+dispy, fcx-dispy)
    f1y <- c(y2, fcy+dispx, fcy-dispx)
    #f2x <- c(c, fcx, fcx)
    #f2y <- c(d, fcy, fcy)
    #arrow.side.length <- sqrt((c-fcx)*(c-fcx)+(d-fcy)*(d-fcy))
    triangles3d(f1x, f1y, c(z,z,z))
    #triangles3d(f2x, f2y, c(z,z-arrow.side.length/2,z+arrow.side.length/2))

}


plot3d.ml <- function(d, layers, axes.scale=c(1,1,1), vertex.label=F, vertex.color=1, vertex.size=10, arrow.length=.3) {
    
    open3d()
    
    # apply filters
    edges <- d$edges[is.element(d$edges$layer,layers),]
    nodes <- d$nodes[is.element(d$nodes$layer,layers),]
    e.x <- edges$x*axes.scale[1]
    e.y <- edges$y*axes.scale[2]
    e.z <- edges$z*axes.scale[3]
    n.x <- nodes$x*axes.scale[1]
    n.y <- nodes$y*axes.scale[2]
    n.z <- nodes$z*axes.scale[3]

#edge.color=edges$directed+1

    # drawing edges
    segments3d(e.x, e.y, e.z)
    # drawing arrows
    for (i in 1:length(edges$directed/2)) {
        #if (edges[i*2,]$directed==TRUE) {
        #    draw.arrow(e.x[i*2-1],e.y[i*2-1],e.x[i*2],e.y[i*2],e.z[i*2],arrow.length)
        #}
    }
    # drawing nodes
    points3d(nodes$x*axes.scale[1], nodes$y*axes.scale[2], nodes$z*axes.scale[3], size=vertex.size, color=vertex.color)
    # drawing labels
    if (vertex.label!=TRUE) {
        text3d(n.x, n.y, n.z, texts=nodes$actor, adj=1.5)
    }
}


# An utility function extracting lists of nodes in the same community, used by the plot function to draw a transparent colored area around each group as it is done in igraph

get.groups.ml <- function(communities) {
	community.ids <- unique(unlist(communities[,3]));
	groups <- vector("list",length(community.ids))
	j <- 1; 
	for (i in community.ids) {
		groups[[j]] <- unlist(unique(communities[communities[,3]==i,][,1]))
		j <- j+1
	}
	groups
}
