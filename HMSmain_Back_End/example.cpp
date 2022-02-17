#include <Arduino.h>

// Pointer and struct example

struct data_arrays
{
  int a;  // address 1
  int b;  // address 2
  int *c; // address 3 storess: address 1
  int *d; // address 4 stores: address 1

  int &e; // address 1
  int data_array[10];
  int data_array_size;
};

data_arrays data;

void foo1(int *a)
{ // address 5 stores: address 1
  *a = 50;
}

void fillArray(int *a, size_t size)
{
  for (int i = 0; i < size; i++)
  {
    a[i] = i * i;
  }
}

// template example
template <size_t N>
void fillArrayRef(int (&arr)[N])
{
  size_t sizeof_array = sizeof(arr) / sizeof(int);
  for (int i = 0; i < sizeof_array; i++)
  {
    data.a[i] = i * i;
  }
}

void fillArrayRef(int (&arr)[20])
{
  size_t sizeof_array = sizeof(arr) / sizeof(int);
  for (int i = 0; i < sizeof_array; i++)
  {
    a[i] = i * i;
  }
}

void fillArrayRef(int (&arr)[30])
{

  size_t sizeof_array = sizeof(arr) / sizeof(int);
  for (int i = 0; i < sizeof_array; i++)
  {
    a[i] = i * i;
  }
}

void test()
{
  data.a = 10; // address 1
  data.b = data.a;       // address 2
  *data.c = &data.a;     // address 3 storess: address 1
  *data.d = data.c;      // address 4 stores: address 1

  int &e = data.a; // address 1
  e = 15;

  *data.c = 30;

  foo1(&data.a);

  int array[20];
  int array1[30];
  fillArray(array, 20);
  fillArrayRef(array);
  fillArrayRef(array1);
}