library("ggplot2")
library("reshape")

LOWER_LIMIT <- 0
UPPER_LIMIT <- 5

####
# Plot shortcuts
####

silent_yaxis <- opts(axis.text.y = theme_blank(), axis.ticks = theme_blank())
no_legend <- opts(legend.position = "none")

limbs <- c("ForeLeft", "HindLeft", "ForeRight", "HindRight")

###
# Get data
###
time_dist <- read.table("result_time.txt", header = TRUE)
trajectory <- read.table("result_rob_trajectory.txt", header = TRUE)
hip_trajectory <- read.table("result_hip_trajectory.txt", header = TRUE)
knee_trajectory <- read.table("result_knee_trajectory.txt", header = TRUE)
leg_trajectory <- read.table("result_leg_trajectory.txt", header = TRUE)
leg_trajectory_rob <- read.table("result_leg_trajectory_rframe.txt", 
	header = TRUE)
forces <- read.table("result_forces.txt", header = TRUE)

trajectory$time = time_dist$time
hip_trajectory$time = time_dist$time
knee_trajectory$time = time_dist$time
leg_trajectory$time = time_dist$time
leg_trajectory_rob$time = time_dist$time

# Restricted ranges:
time_dist.r = subset(time_dist, distance > LOWER_LIMIT & 
	distance < UPPER_LIMIT)
trajectory.r = subset(trajectory, distance > LOWER_LIMIT & 
	distance < UPPER_LIMIT)
hip_trajectory.r = subset(hip_trajectory, distance > LOWER_LIMIT & 
	distance < UPPER_LIMIT)
knee_trajectory.r = subset(knee_trajectory, distance > LOWER_LIMIT & 
	distance < UPPER_LIMIT)
leg_trajectory.r = subset(leg_trajectory, distance > LOWER_LIMIT & 
	distance < UPPER_LIMIT)
leg_trajectory_rob.r = subset(leg_trajectory_rob, 
	distance > LOWER_LIMIT & distance < UPPER_LIMIT)
forces.r = subset(forces, distance > LOWER_LIMIT & 
	distance < UPPER_LIMIT)



##############################################################################
# Plot
##############################################################################

# qplot(time, distance, data = time_dist, geom = "line")

############
### ACTUATOR ANALYSIS
###########

# a function to reshape the data for plotting.
# frm: position dataframe
# joint_letter: 'K' for knee, 'H' for hip
pos_reshape <- function(frm, joint_letter) {
	# melt into distance, var, and value:
	frm.m <- melt(frm, id=c('distance', 'time'))
	# split variables into type ('set' or 'real') and limb ('ForeRight', etc.)
	# strsplit splits on *all* H; replace the first with X
	frm.m <- within(frm.m, variable <- factor(sub(joint_letter, 'X', variable)))
	split_list <- strsplit(as.character(frm.m$variable), 'X')
	# convert to matrix for easier access to columns
	split_mat <- matrix(unlist(split_list), ncol = 2, byrow = TRUE)
	# create a new dataframe, listing actuator position as a function of limb and
	# type
	frm.s = data.frame(distance = frm.m$distance, 
		time = frm.m$time,
		type = factor(split_mat[,1]),
		limb = factor(split_mat[,2]))
	frm.s[paste(joint_letter, 'pos', sep = '')] = frm.m$value # Hpos or Kpos

	return( frm.s )
}

# HIPS 
hip.s <- pos_reshape(hip_trajectory.r, 'H')
p_hip_error <- qplot(time, Hpos, colour = type, data = hip.s, geom = 'line',
	xlab = "Time [s]", ylab = "Hip Angle [rad]", 
	main = "Hip angle error") + facet_grid(limb ~ .) + no_legend

# KNEES
knee.s <- pos_reshape(knee_trajectory.r, 'K')
p_knee_error <- qplot(time, Kpos, colour=type, data = knee.s, geom='line',
	xlab = "Time [s]", ylab = "Knee Angle [rad]",
	main = "Knee angle error") + facet_grid(limb ~ .) + no_legend

# make a big data frame with Hpos and Kpos as a function of distance, limb, and
# type
# TODO make sure the merge actually worked right (by making the four plots
# individually)
joint_pos <- merge(hip.s, knee.s, by=c("distance", "limb", "type"))
real_pos <- subset(joint_pos, type == 'real' & distance > 0.5)
p_pos_phase <- qplot(Hpos, Kpos, data = real_pos, geom = 'path', colour = distance,
	xlab = "Hip Angle [rad]", ylab = "Knee Angle [rad]", 
	main = "Angle Phase Plot") + facet_wrap(~ limb, scales = 'free') +
	no_legend

############
### POSITION ANALYSIS
###########

### PITCH AND ROLL 
# phase plot
p_pitch_vs_roll <- qplot(roll, pitch, data = trajectory.r, geom = 'path', 
	size = 0.5, alpha = 0.5, colour = distance, xlab = "Roll", ylab = "Pitch",
	main = "Pitch - Roll phaseplot") + no_legend

# vs time
p_pitch <- qplot(time, pitch, data = trajectory, geom = 'path',
	xlab = "Time [s]", ylab = "Pitch")
p_roll <- qplot(time, roll, data = trajectory, geom = 'path',
	xlab = "Time [s]", ylab = "Roll")

### TRAJECTORY PLOT
								
# Map view
trajectory$in_range = factor(as.integer(trajectory$distance < LOWER_LIMIT) + 
	as.integer(trajectory$distance < UPPER_LIMIT))
asp <- max(abs(trajectory$z)) / max(abs(trajectory$x)) # aspect ratio for plot
ymin <- min(abs(trajectory$x), abs(trajectory$z))
ymax <- max(abs(trajectory$x), abs(trajectory$z))
yshift <- mean(c(ymin, ymax))
p_traj <- qplot(x, -z, data = trajectory, colour = in_range, 
	geom = "line", ylab = "y", main = "Trajectory") +
	ylim(ymin - yshift, ymax - yshift) + 
	scale_colour_manual("In Range", values = c("black", "red", "black")) +
	no_legend

############
### MAD MAN PLOT!
# places robot position, foot position, and foot forces, all in one.

## CM
# extract just distance and y, and rename to yCM
# CM <- trajectory.r[,c('distance', 'y')]
# CM <- rename(CM, c(y = 'yCM'))

# ## positions
# # Get y values from world reference, x values from robot reference
# sel <- c('distance', grep('^y', names(leg_trajectory.r), value = TRUE))
# leg.y <- leg_trajectory.r[,sel]
# sel <- c('distance', grep('^x', names(leg_trajectory.r), value = TRUE))
# leg.x <- leg_trajectory_rob.r[,sel]
# foot.pos <- merge(leg.x, leg.y)

# # melt and split variable into limb and direction (x or y)
# # ugly hack, for splitting later on
# foot.pos.m <- melt(foot.pos, id="distance")
# foot.pos.m <- within(foot.pos.m, variable <- sub('x', 'xS', variable))
# foot.pos.m <- within(foot.pos.m, variable <- sub('y', 'yS', variable))
# foot.pos.m <- cbind(foot.pos.m, 
	# colsplit(foot.pos.m$variable, 'S', names = c("dir", "limb")))
# foot.pos <- cast(foot.pos.m, distance + limb ~ dir)

# ## Forces; same procedure as positions.
# sf <- 0.0000005	 # Scaling factor for the force
# foot.force.m <- melt(forces.r, id="distance")
# foot.force.m <- within(foot.force.m, variable <- sub('fx', 'fxS', variable))
# foot.force.m <- within(foot.force.m, variable <- sub('fy', 'fyS', variable))
# foot.force.m <- cbind(foot.force.m, 
	# colsplit(foot.force.m$variable, 'S', names = c("dir", "limb")))
# foot.force <- cast(foot.force.m, distance + limb ~ dir)

# ### Plot
# pos <- merge(CM, foot.pos, id = "distance")
# pos_force <- merge(pos, foot.force, id = c("distance", "limb"))
# # draw segments and force vectors only every 20 points 
# pos_force.s <- pos_force[order(pos_force$limb),]
# pos_force.s <- subset(pos_force.s, (1:nrow(pos_force.s) %% 100) == 1)
# p_all <- ggplot(pos_force, aes(distance)) + ylab("Height") + 
	# # foot path
	# geom_path(aes(x = distance + x, y = yCM + y)) +
	# # leg segment
	# geom_segment(aes(x = distance + x, y = yCM + y, xend = distance, yend = yCM),
			# colour = grey(0.4), data = pos_force.s) + 
	# # CM
	# geom_path(aes(y = yCM), size = 1.5) +
	# ## forces
	# # geom_segment(aes( 	
			# # x = distance + x, y = yCM + y, 
			# # xend = distance + x + sf*fx, yend = yCM + y + sf*fy), 
			# # data = pos_force.s, arrow = arrow(length=unit(0.1, "cm"))) + 
	# facet_grid(limb ~ .) + no_legend

# # CM over distance
p_cm <- qplot(time, y, data = trajectory.r, geom = 'path',
	xlab = "Time [s]", ylab = "Height")

### FOOT FORCE PLOT
# Filter data: considered as touching the ground if force superior to threshold
threshold <- 0
forces.n <- forces.r
sel <- grep('^fy', names(forces.n), value = TRUE)
forces.n[sel] <- lapply(forces.n[sel], '>', threshold)
print(mean(forces.n[sel]))

# I only want forces in the y direction
forces.y <- forces.n[c('distance', sel)]
forces.m = melt(forces.y, id = "distance", rm.na = FALSE)
# drop 'fy' to use variable name as a label.
forces.m$variable <- sub('fy', '', forces.m$variable)
# colour according to swing and stance
p_force <- qplot(distance, variable, data = forces.m, fill = value, 
	width= 0.002, geom = 'tile') + 
	scale_fill_manual("", values = c(gray(0.95), gray(0.2)),
		breaks = c(FALSE, TRUE), labels = c("Swing", "Stance"))  +
	ylim(rev(limbs)) + 
	scale_x_continuous("Distance", expand = c(0,0)) + 
	scale_y_discrete("Limb", expand = c(0,0)) +
	theme_bw()
	#opts(panel.background = theme_blank(), axis.line = theme_segment())
 p_force

############
### FINAL LAYOUT
###########

# REMINDER: plots I have:
# p_cm	
# p_force	
# p_pitch	
# p_pitch_vs_roll	
# p_roll	
# p_traj	
# p_all

# p_hip_error	
# p_knee_error	
# p_pos_phase	

### Put them all together in a nice grid
vplayout <- function(x, y)
	viewport(layout.pos.row = x, layout.pos.col = y)

pdf("plots.pdf", width = 11.7 * 1.5, height=8.3 * 1.5)
# The everything plot
# grid.newpage()
# print(p_all)

# Trajectory analysis
grid.newpage()
pushViewport(viewport(layout = grid.layout(5, 5)))
print(p_cm, vp = vplayout(1, 1:3))
print(p_pitch, vp = vplayout(2, 1:3))
print(p_roll, vp = vplayout(3, 1:3))
print(p_force, vp = vplayout(4:5, 1:3))
print(p_traj, vp = vplayout(1:2, 4:5))
print(p_pitch_vs_roll, vp = vplayout(3:5, 4:5))


# Actuator analysis
grid.newpage()
pushViewport(viewport(layout = grid.layout(2, 2)))
vplayout <- function(x, y)
	viewport(layout.pos.row = x, layout.pos.col = y)
print(p_hip_error, vp = vplayout(1, 1))
print(p_knee_error, vp = vplayout(2, 1))
print(p_pos_phase, vp = vplayout(1, 2))

dev.off()

pdf("poor_trajectory.pdf", width = 16, height = 10)
grid.newpage()
pushViewport(viewport(layout = grid.layout(3, 1)))
print(p_cm, vp = vplayout(1, 1))
print(p_pitch, vp = vplayout(2, 1))
print(p_roll, vp = vplayout(3, 1))
dev.off()

pdf("poor_actuators.pdf", width = 16, height = 12)
grid.newpage()
pushViewport(viewport(layout = grid.layout(2, 1)))
print(p_hip_error, vp = vplayout(1, 1))
print(p_knee_error, vp = vplayout(2, 1))
dev.off()

pdf("poor_footfall.pdf", width = 16, height = 6)
print(p_force, vp = vplayout(1, 1))
dev.off()
