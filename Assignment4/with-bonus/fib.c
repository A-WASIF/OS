/*
 * No changes are allowed in this file
 */
char arr [5001];
int fib(int n) {
  if(n<2) return n;
  else return fib(n-1)+fib(n-2);
}

int _start() {
	arr[1000] = 'a';
	arr[5000] = 'b';
  int val = fib(10);
	return val;
}
