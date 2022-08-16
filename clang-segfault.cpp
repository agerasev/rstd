template <const int *P>
const int *a() {
    return P;
}

template <typename T>
T b(T x) {
    static constexpr int N[] = {0};
    a<N>();
    return x;
}

int main() {
    return b(0);
}
