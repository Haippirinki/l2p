def lerp(value_1, value_2, t):
	return value_1 * (1.0 - t) + value_2 * t

def arc(time, n, radius_1, radius_2, angle_1, angle_2, speed, enemy_type="basic"):
	for i in range(n):
		t = float(i) / float(n - 1)
		radius = lerp(radius_1, radius_2, t)
		angle = lerp(angle_1, angle_2, t)
		print enemy_type, time, radius, angle, speed

def circle(time, n, radius, angle, speed, enemy_type="basic"):
	arc(time, n, radius, radius, angle, angle + 360.0 * (n - 1) / n, speed, enemy_type)
