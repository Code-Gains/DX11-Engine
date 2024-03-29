struct Int3 {
    int x;
    int y;
    int z;

    Int3() { x = 0; y = 0; z = 0; };
    Int3(const int x, const int y, const int z);
    ~Int3() {}

    Int3 operator+(const Int3& other) const;
    Int3 operator-(const Int3& other) const;
};