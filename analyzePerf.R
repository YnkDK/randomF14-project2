library(lattice)
library(xtable)
# Read and format our performance 
D <- read.table("performance.out", fill = TRUE)
colnames(D) <- c("Program", "dataA", "dataB", "seed", "time", "response")

# Convert from nanoseconds to milliseconds
D$time <- D$time/1000000.0
D$n <- 10^as.numeric(gsub("\\D", "", D$dataA))
D$Program <- factor(D$Program)
# Remove top 5% outliers
D <- D[which(D$time <= quantile(D$time, 0.95)),]
# Set output file
pdf("graphs.pdf", width=11.7, height=8.3)
par(mar=c(5,3,2,2)+0.1)

# Make sure all points are unique
trellis.par.set(superpose.symbol=list(pch = 1:length(unique(D$Program))))
dataPlot <- xyplot(time ~ n,
                   groups = Program,
                   data = D,
                   type = c("a", "p"),
                   scales = list(x = list(log = 10),
                                 y = list(log = FALSE)),
                   xlab = "n", 
                   ylab = "Time (ms)",
                   ylim = c(0, 4500),
                   auto.key = TRUE)

print(dataPlot, width=200)
# Close the dev
silence = dev.off()

D$response <- ifelse(D$response == "Yes", 1, 0)

s <- aggregate(response ~ Program + n, data = D, FUN = 'sum')
s <- s[order(s$Program, s$n),]
table <- xtable(s, digits = 0)

print(table, type= "latex", include.rownames=FALSE)