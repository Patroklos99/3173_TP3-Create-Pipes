#!/usr/bin/env bats

load test_helper

@test "gratuit" {
	true
}

@test "fail_sortie_signal" { 	
	run ./tubeo echo "INF3173 - Principes des systèmes d'exploitation" : 	 	 check 139 "" 
}
