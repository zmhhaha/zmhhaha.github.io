### geometry3D.h文件

#### Box3D

```c++
struct Box3D {
    Box3D() : x0(), x1(), y0(), y1(), z0(), z1() { }
    Box3D(plint x0_, plint x1_, plint y0_, plint y1_, plint z0_, plint z1_)
        : x0(x0_), x1(x1_), y0(y0_), y1(y1_), z0(z0_), z1(z1_)
    { }
    /// Add a border of nCells cells to the box
    Box3D enlarge(plint nCells) const {
        return Box3D(x0-nCells, x1+nCells,
                     y0-nCells, y1+nCells,
                     z0-nCells, z1+nCells);
    }

    /// Add a border of nCells cells to the box in one direction (x=0, y=1, z=2)
    Box3D enlarge(plint nCells, plint dir) const {
        switch ( dir ) {
            case 0:
              return Box3D(x0-nCells, x1+nCells, y0, y1, z0, z1);
            case 1:
              return Box3D(x0, x1, y0-nCells, y1+nCells, z0, z1);
            case 2:
              return Box3D(x0, x1, y0, y1, z0-nCells, z1+nCells);
            default:
              PLB_ASSERT(false && "The direction must be 0, 1, or 2");
              break;
        }
        return Box3D(-1,-1,-1,-1,-1,-1);
    }
    /// Number of cells in x-direction
    plint getNx()  const { return (x1-x0+1); }
    /// Number of cells in y-direction
    plint getNy()  const { return (y1-y0+1); }
    /// Number of cells in z-direction
    plint getNz()  const { return (z1-z0+1); }
    /// Total number of cells in the box
    plint nCells() const { return getNx()*getNy()*getNz(); }
    /// Return the maximum of getNx(), getNy(), and getNz()
    plint getMaxWidth() const { return std::max(std::max(getNx(), getNy()), getNz()); }

    bool operator==(Box3D const& rhs) const {
        return x0 == rhs.x0 && y0 == rhs.y0 && z0 == rhs.z0 &&
               x1 == rhs.x1 && y1 == rhs.y1 && z1 == rhs.z1;
    }

    plint x0, x1, y0, y1, z0, z1;
};
```

Box3D用于规定数据操作范围，常被作为参数使用，enlarge系列用于扩大和减小Box范围，get系列用于获得相应的参数。



#### Dot3D

```c++
struct Dot3D {
    Dot3D() : x(), y(), z() { }
    Dot3D(plint x_, plint y_, plint z_) : x(x_), y(y_), z(z_) { }
    plint x, y, z;
};
```

Dot3D 由三个成员数据组成分别为x，y，z 。可看做普通点类使用。



#### DotList3D

```c++
struct DotList3D {
    DotList3D() { }
    DotList3D(std::vector<Dot3D> const& dots_) : dots(dots_) { }
    /// Add one more point to the list
    void addDot(Dot3D dot) {
        dots.push_back(dot);
    }
    /// Add more points to the list
    void addDots(std::vector<Dot3D> const& dots_) {
        dots.insert(dots.end(), dots_.begin(), dots_.end());
    }
    /// Get const reference to one of the dots
    Dot3D const& getDot(plint whichDot) const {
        PLB_PRECONDITION( whichDot<getN() );
        return dots[whichDot];
    }
    /// Get non-const reference to one of the dots
    Dot3D& getDot(plint whichDot) {
        PLB_PRECONDITION( whichDot<getN() );
        return dots[whichDot];
    }
    /// Get total number of points
    plint getN() const {
        return dots.size();
    }

    std::vector<Dot3D> dots;
};
```

DotList3D 用一个 vector 容器存储 Dot3D 点数据，常用做规定数据操作范围，常被用作参数。addDot函数用于添加点，getDot函数用于获得点。

