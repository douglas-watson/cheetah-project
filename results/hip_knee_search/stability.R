library("ggplot2")
library("playwith")
library("reshape")

data = read.table("hip-knee.dat", header = TRUE)

data$amplitudeForeHip = factor(data$amplitudeForeHip)
# correct for number of steps.
data$stability = data$stability / 500
data$speed = data$distance / data$duration
data.s = subset(data, duration > 3)
attach(data)

####################
# Helpers
####################
twodigit <- function(x) {
	format(as.numeric(x), digits = 2)
}

################
# Influence of stability
########################
# Distribution of stability:
qplot(stability, data = data, geom = "histogram", binwidth = 1/100)
ggsave("stability-distribution.png", width = 10, height = 6)

qplot(duration, data = data, geom = "histogram") 
ggsave("duration-distribution.png", width = 10, height = 6)

# Stability should increase duration...
qplot(stability, duration, data = data, alpha = 0.6,
	main = "Duration vs Stability", xlab = "Stability", ylab = "Duration") +
	geom_smooth(alpha = 0.5) +
	# scale_colour_discrete("Hip Amplitude") + 
	xlim(0.75, 1)
ggsave("stability-duration.png", width = 10, height = 6)

# and indeed the correlation is striking.
with(data, cor.test(stability, duration, method = "kendall"))

qplot(stability, distance, data = data, colour = amplitudeForeHip,
	alpha = 0.6,
	main = "Distance vs Stability", xlab = "Stability", ylab = "Distance") +
	geom_smooth(aes(group = amplitudeForeHip), alpha = 0.5) +
	scale_colour_discrete("Hip Amplitude") + 
	xlim(0.8, 1)
# ggsave("distance-stability.png", plot = p, width = 10, height = 6)

### EFFECT ON SPEED
qplot(stability, speed, data = data, colour = duration, alpha = 0.6,
	main = "Speed vs Stability", xlab = "Stability", ylab = "Speed") +
	xlim(0.8, 1)
ggsave("stability-speed.png", width = 10, height = 6)

cor.test(stability, speed, method = "spearman")

data.s = subset(data, stability > 400)
qplot(stability, log(duration), data = data, alpha = 0.5) +
	geom_smooth(method = "lm") + ylim(0, 3) + xlim(0.8, 1)

# correlations
#########################
corrdata = data[c(
		'stability', 'duration', 'distance', 'speed',
		'amplitudeHindHip', 'amplitudeForeKnee', 'amplitudeHindKnee'
		)]
corrdata = rename(corrdata, c(amplitudeHindHip = "amplitudeHip"))

cors = cor(corrdata, method = "spearman")
write.table(cors, file = "correlations.dat", sep = "\t")

with(data, cor(stability, log(duration), method = "spearman"))
with(data, cor.test(stability, duration, method = "kendall"))


qplot(amplitudeForeHip, speed, data = data, geom = "boxplot",
	main = "Speed vs Hip amplitude", xlab = "Hip Amplitude", 
	ylab = "Average Speed") +
	stat_summary(fun.y = max, colour = "red", geom = "point") +
	scale_x_discrete(formatter = twodigit)
ggsave("hip-amplitude-speed.png", width = 10, height = 6)

# Control for duration
data.s = subset(data, duration == 15)
with(data.s, cor.test(amplitudeHindHip, distance/duration), method = "spearman")

qplot(amplitudeForeHip, distance/duration, data = data.s, geom = "boxplot",
	main = "Speed vs Hip amplitude", xlab = "Hip Amplitude", 
	ylab = "Average Speed") +
	stat_summary(fun.y = max, colour = "red", geom = "point") +
	scale_x_discrete(formatter = twodigit)
ggsave("hip-amplitude-speed_15sec.png", width = 10, height = 6)

# Amusingly, most stability values are above ...
qplot(stability, data = data.s, geom = "histogram", binwidth = 1/100) +
	xlim(0, 1)

########################
# Knees
########################

# Not much effect of knee amplitude on stability.
qplot(amplitudeForeKnee, duration, data = data, geom = "jitter", alpha = 0.3)
qplot(amplitudeForeKnee, stability, data = data, geom = "smooth")
qplot(amplitudeForeKnee, distance, data = data, geom = "smooth")
# ggsave("knee-amplitude-balance.png", plot = p, width = 10, height = 6)

# no correlation with duration (balance)
with(data, cor.test(amplitudeForeKnee, duration, method = "spearman"))
# significant slightly decreasing stability with higher hip amplitude: when
# knees are bent futher, the robot rocks further.
with(data, cor.test(amplitudeForeKnee, distance, method = "spearman"))
with(data, cor.test(amplitudeForeKnee, stability, method = "spearman"))
with(data, cor.test(amplitudeForeKnee, speed, method = "spearman"))

qplot(factor(amplitudeForeKnee), speed, data = data.s, geom = "boxplot",
	xlab = "Fore Knee Amplitude", ylab = "Speed" ) +
	scale_x_discrete(formatter = twodigit)
ggsave("speed-ampForeKnee-15sec.png", width = 10, height = 6)

qplot(factor(amplitudeForeKnee), stability, data = data.s, geom = "boxplot",
	xlab = "Fore Knee Amplitude", ylab = "Stability") +
	ylim(0.93, 1) + scale_x_discrete(formatter = twodigit)
ggsave("stability-ampForeKnee-15sec.png", width = 10, height = 6)
qplot(factor(amplitudeForeKnee), duration, data = data, geom = "boxplot")


### HIND KNEES
# for this analysis, I'll discard the variables I'm never using
# There seems to be an increase in maximum distance with higher hip amplitudes
qplot(factor(amplitudeHindKnee), distance, data = data, geom = "boxplot")

# what about speed?
qplot(factor(amplitudeHindKnee), speed, data = data, geom = "boxplot")

corrdata.m = melt(corrdata, id = c("amplitudeHip", "amplitudeForeKnee",
		"amplitudeHindKnee"))
corrdata.max = cast(corrdata.m, amplitudeHindKnee ~ variable, max)
# This one has the *index* of the best value, so we can see what max values go
# together
corrdata.whichmax = cast(corrdata.m, amplitudeHindKnee ~ variable, which.max)

# Actually we can see if there's any correlation with any variable
corrdata.max.m = melt(corrdata.max, id = "amplitudeHindKnee")
qplot(amplitudeHindKnee, value, data = corrdata.max.m) +
	facet_grid(variable ~ ., scale = "free")
with(corrdata.max, cor.test(amplitudeHindKnee, distance, method = "spearman"))
with(corrdata.max, cor.test(amplitudeHindKnee, speed, method = "spearman"))
with(corrdata.max, cor.test(amplitudeHindKnee, stability, method = "spearman"))

### Max values for hip amplitudes
corrdata.maxhip = cast(corrdata.m, amplitudeHip ~ variable, max)
qplot(amplitudeHip, speed, data = corrdata.maxhip)
with(corrdata.maxhip, cor.text(amplitudeHip, speed, method = "spearman"))

corrdata.thirty = subset(corrdata, amplitudeHip == 0.30)
# max speed in this
corrdata.thirty.m = melt(corrdata.thirty, id = "amplitudeHindKnee")
corrdata.thirty.max = cast(corrdata.thirty.m, amplitudeHindKnee ~ variable,
	max)
qplot(factor(amplitudeHindKnee), speed, data = corrdata.thirty.max, 
	geom = "point")
with(corrdata.thirty.max, cor.test(amplitudeHindKnee, speed))
aovdata = aov(distance * duration ~ amplitudeHindKnee * amplitudeForeKnee * amplitudeHip,
	data = corrdata)
write.table(aovdata, file = "anova_dist-duration.dat", sep = "\t")
