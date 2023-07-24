struct Int3 {
    int x;
    int y;
    int z;

    Int3() {};
    Int3(const int x, const int y, const int z);
    ~Int3();

    Int3 operator+(const Int3& other) const;
    Int3 operator-(const Int3& other) const;
};