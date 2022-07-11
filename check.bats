#!/usr/bin/env bats

load test_helper

@test "simple" {
	run ./tubeo echo "INF3173 - Principes des systèmes d'exploitation"
	check 0 "INF3173 - Principes des systèmes d'exploitation"
}

@test "fail" {
	run ./tubeo fail "INF3173 - Principes des systèmes d'exploitation"
	check 127 ""
}

@test "pipe" {
	run ./tubeo echo "INF3173 - Principes des systèmes d'exploitation" : wc
	check 0 "      1       6      49"
}

@test "pipe-count" {
	run bash -c "./tubeo -n 1 echo INF3173 - Principes des OS : wc | sort"
	checki 0 <<FIN
      1       5      27
27
FIN
}

@test "pipeline" {
	run ./tubeo echo "INF3173 - Principes des systèmes d'exploitation" : wc : rev : cat
	checki 0 <<FIN
94      6       1      
FIN
}

@test "pipeline-count" {
	run bash -c "./tubeo -n 3 echo INF3173 - Principes des OS : wc : rev : cat | sort"
	checki 0 <<FIN
24
72      5       1      
FIN
}

@test "fail-pipeline" {
	run ./tubeo -n 3 echo "INF3173 - Principes des systèmes d'exploitation" : fail : rev : cat
	check 0 "0"
}

@test "long-pipeline" {
	run ./tubeo echo "INF3173 - Principes des systèmes d'exploitation" : wc : wc : wc : wc : wc : wc : wc : wc : wc
	check 0 "      1       3      24"
}
