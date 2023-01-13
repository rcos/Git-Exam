exec("""
def cl():
	return Course('PHIL', 4420, 'Computability & Logic', time(hour=10, minute=0), time(hour=11, minute=50), set([DayOfWeek.TUESDAY, DayOfWeek.FRIDAY]))

def ece():
	return Course('ARCH', 2370, 'Energy, Comfort, & Ecology', time(hour=10, minute=0), time(hour=11, minute=50), set([DayOfWeek.WEDNESDAY]))

def ep():
	return Course('PHYS', 2350, 'Experimental Physics', time(hour=8, minute=0), time(hour=11, minute=50), set([DayOfWeek.WEDNESDAY]))
""")
