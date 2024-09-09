#include <bits/stdc++.h>
using namespace std;

const int N = 1e6 + 10;

int a[N];

// 生成随机数
int randint(int l, int r) {
    return rand() % (r - l + 1) + l;
}

// 三路快速排序的分区函数
void partition3(int* a, int p, int q, int& lt, int& gt) {
    int pivot = a[p];
    lt = p;       // lt 初始指向第一个元素
    gt = q;       // gt 初始指向最后一个元素
    int i = p + 1; // i 初始指向第二个元素

    while (i <= gt) {
        if (a[i] < pivot) {
            swap(a[i], a[lt]);
            lt++;
            i++;
        } else if (a[i] > pivot) {
            swap(a[i], a[gt]);
            gt--;
        } else {
            i++;
        }
    }
}

// 三路快速排序函数
void quicksort3(int* a, int p, int q) {
    if (p >= q) return;

    // 随机选择枢轴并将其交换到第一个位置
    int pivotIndex = randint(p, q);
    swap(a[p], a[pivotIndex]);

    int lt, gt;
    partition3(a, p, q, lt, gt);

    quicksort3(a, p, lt - 1);
    quicksort3(a, gt + 1, q);
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    int n;
    cin >> n;
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }

    quicksort3(a, 0, n - 1);

    for (int i = 0; i < n - 1; i++) {
        cout << a[i] << " ";
    }
    cout << a[n - 1] << "\n";

    return 0;
}
