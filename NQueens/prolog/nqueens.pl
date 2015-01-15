/*
	N-Queen program
	L. Sterling and E. Shapiro: "The Art of Prolog"
	(Chapter 14)
*/
main :-
	write('N = '),
	read(N),
	statistics(runtime, _),
	queen_all(N),
	statistics(runtime, [_,T]),
	write('CPU time = '), write(T), write(' msec'), nl.

queen_all(N) :-
	queens(N, Q),
%	write(Q), nl,
	fail.
queen_all(_).

queens(N, Qs) :-
	range(1, N, Ns),
	queens(Ns, [], Qs).

queens(UnplacedQs, SafeQs, Qs) :-
	select(Q, UnplacedQs, UnplacedQs1),
	\+ attack(Q, SafeQs),
	queens(UnplacedQs1, [Q|SafeQs], Qs).
queens([], Qs, Qs).

select(X, [X|Xs], Xs).
select(X, [Y|Ys], [Y|Zs]) :- select(X, Ys, Zs).

attack(X, Xs) :-
	attack(X, 1, Xs).

attack(X, N, [Y|Ys]) :-
	X is Y + N.
attack(X, N, [Y|Ys]) :-
	X is Y - N.
attack(X, N, [Y|Ys]) :-
	N1 is N + 1,
	attack(X, N1, Ys).

range(M, N, [M|Ns]) :-
	M < N, M1 is M + 1, range(M1, N, Ns).
range(N, N, [N]).
