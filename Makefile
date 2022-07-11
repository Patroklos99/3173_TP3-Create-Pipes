all: tubeo

check: tubeo
	bats check.bats
clean:
	rm tubeo
