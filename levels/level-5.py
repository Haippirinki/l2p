import gen

print "Same but twice as fast"

BASE = 1
BASE_ANGLE = 22.5
BASE_SPEED = 1
def inc():
    global BASE
    BASE += 2
    return BASE

gen.circle(BASE, 8, 2, 0, BASE_SPEED)
gen.circle(inc(), 8, 2, BASE_ANGLE, BASE_SPEED)
gen.circle(inc(), 8, 2, 2 * BASE_ANGLE, BASE_SPEED)
gen.circle(inc(), 8, 2, 3 * BASE_ANGLE, BASE_SPEED)
gen.circle(inc(), 8, 2, 4 * BASE_ANGLE, BASE_SPEED)
