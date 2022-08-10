#!/usr/bin/env bats

load test_helper

@test "sieste" {
	run ./tubeo echo Bonjour : sleep 0.5 : echo Monde : cat
	check 0 "Monde"
}

@test "long-pipeline" {
	run ./tubeo echo "INF3173 - Principes des systèmes d'exploitation" : cat : cat : cat : cat : cat : cat : cat : cat : cat : cat
	check 0 "INF3173 - Principes des systèmes d'exploitation"
}

@test "end-fail" {
	run ./tubeo echo "INF3173 - Principes des systèmes d'exploitation" : cat : cat : fail
	check 127 ""
}

@test "end-fail-middle-n" {
	run ./tubeo -n 2 echo "INF3173 - Principes des systèmes d'exploitation" : cat : cat : fail
	check 127 "49"
}

@test "nada" {
	run ./tubeo sleep 0.1 : sleep 0.5 : sleep 0.1
	check 0 ""
}

@test "stdin" {
	echo "bonjour le monde" > /tmp/bijour
	run bash -c "./tubeo cat : wc -w < /tmp/bijour"
	check 0 "3"
}

@test "stdout" {
	run bash -c "./tubeo > /tmp/out echo bonjour le monde"
	check 0 ""
	test "bonjour le monde" = "$(cat /tmp/out)"
}

@test "signal" {
	run ./tubeo bash -c 'kill -n SIGTERM $$'
	check 143 ""
}

@test "fd-milieu" {
	run ./tubeo echo Bijour : cat : ls /proc/self/fd/ : wc -l
	check 0 "5"
}
