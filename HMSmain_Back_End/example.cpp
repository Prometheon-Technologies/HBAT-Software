

void foo1(int *a) { // address 5 stores: address 1
  *a = 50;
}

void fillArray(int *a, size_t size) {
  for (int i=0; i<size; i++) {
    a[i] = i*i;
  }
}

template<size_t N>
void fillArrayRef(int (&arr)[N]) {
  size_t sizeof_array = sizeof(arr) / sizeof(int);
  for (int i=0; i<sizeof_array; i++) {
    a[i] = i*i;
  }
}

void fillArrayRef(int (&arr)[20]) {
  size_t sizeof_array = sizeof(arr) / sizeof(int);
  for (int i=0; i<sizeof_array; i++) {
    a[i] = i*i;
  }
}

void fillArrayRef(int (&arr)[30]) {
  size_t sizeof_array = sizeof(arr) / sizeof(int);
  for (int i=0; i<sizeof_array; i++) {
    a[i] = i*i;
  }
}

void test() {
  int a = 10; // address 1
  int b = a; // address 2
  int * c = &a; // address 3 storess: address 1
  int * d = c; // address 4 stores: address 1

  int &e = a; //address 1
  e = 15;

  *c = 30;

  foo1(&a);

  int array[20];
  int array1[30];
  fillArray(array, 20);
  fillArrayRef(array);
  fillArrayRef(array1);
}