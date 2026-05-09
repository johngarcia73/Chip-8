#include<stdio.h>
#include<iostream>
#include"chip_8.cpp"

int main(int argc, char const *argv[])
{
    Chip_8 chip;
    cout<<static_cast<int>(chip.getStackCell(5000));
    return 0;
}
