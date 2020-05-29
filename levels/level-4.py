import gen

print "Just a few more waves"

BASE = 1
BASE_ANGLE = 22.5

def inc():
    global BASE
    BASE += 2
    return BASE

gen.circle(BASE, 8, 2, 0, 0.5)
gen.circle(inc(), 8, 2, BASE_ANGLE, 0.5)
gen.circle(inc(), 8, 2, 2 * BASE_ANGLE, 0.5)
gen.circle(inc(), 8, 2, 3 * BASE_ANGLE, 0.5)
gen.circle(inc(), 8, 2, 4 * BASE_ANGLE, 0.5)
