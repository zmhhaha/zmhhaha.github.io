# Permeability案例
#### 逐行注释解析
**文件位置：/palabos-v2.0r0/examples/tutorial/permeability.cpp**


```cpp
#include "palabos3D.h"
#include "palabos3D.hh"

#include <vector>
#include <cmath>
#include <cstdlib>

using namespace plb;

typedef double T;
#define DESCRIPTOR descriptors::D3Q19Descriptor 
//格子描述符，用于规范计算域内每个元胞内的数据及偏移值，数据是以连续表的形式存储于计算域当中

// This function object returns a zero velocity, and a pressure which decreases
//   linearly in x-direction. It is used to initialize the particle populations.

//提供一个压力梯度类，后文以函数指针方式调用，运用设计模式中的命令模式（ps：初学者不用看这条解释）
class PressureGradient {
public:
    PressureGradient(T deltaP_, plint nx_) : deltaP(deltaP_), nx(nx_)
    { }
    
    //运算符重载
    void operator() (plint iX, plint iY, plint iZ, T& density, Array<T,3>& velocity) const
    {
        velocity.resetToZero();
        density = (T)1 - deltaP*DESCRIPTOR<T>::invCs2 / (T)(nx-1) * (T)iX;
    }
private:
    T deltaP;
    plint nx;
};

//读取结构的函数，传入文件读取名称，传出3D标量场数据
void readGeometry(std::string fNameIn, std::string fNameOut, MultiScalarField3D<int>& geometry)
{
    const plint nx = geometry.getNx();
    const plint ny = geometry.getNy();
    const plint nz = geometry.getNz();

    Box3D sliceBox(0,0, 0,ny-1, 0,nz-1); //详情请见geometry3D.h文件
    //Box3D是用于规定操作执行区域，一般每个对数据操作都会用其作为参数，此处选取yz面作为输入基准
    //但输入数据有很多种其他的写法，初学者按照这个仿写就可以，C++学好了，再尝试新方法
    
    std::auto_ptr<MultiScalarField3D<int> > slice = generateMultiScalarField<int>(geometry, sliceBox);
    //std::auto_ptr智能指针，用于在使用特定堆对象后，自动释放内存防止内存泄漏
    //generateMultiScalarField函数详见multiBlockGenerator文件系列，用于在堆区新建对象
    
    plb_ifstream geometryFile(fNameIn.c_str()); 
    //Palabos重载的输入流函数，直接用即可
    
    //一片一片流输入数据，因为原案例是从图来的，所以输入写成这样
    //自己的数据如果是一个数据块，写可以直接写一个三重循环的流输入，不用那么麻烦
    for (plint iX=0; iX<nx-1; ++iX) {
        if (!geometryFile.is_open()) {
            pcout << "Error: could not open geometry file " << fNameIn << std::endl;
            exit(EXIT_FAILURE);
        }
        //流输入，slice此处是一个指针，也是一个数组，*ptr与ptr[]是同等操作（C++ Primer）
        geometryFile >> *slice;
        
        //copy函数，详见copyData.cpp文件，用于将相同的场数据进行复制转移
        //此处是用于转移标量场数据
        copy(*slice, slice->getBoundingBox(), geometry, Box3D(iX,iX, 0,ny-1, 0,nz-1));
    }

    {
    	//vtk输出部分，此处就是直接用，在别的地方以及数据输出也都差不多
        VtkImageOutput3D<T> vtkOut("porousMedium", 1.0);
        vtkOut.writeData<float>(*copyConvert<int,T>(geometry, geometry.getBoundingBox()), "tag", 1.0);
    }

	//stl输出部分，因为我不涉及stl，所以我并没有看，一般我会把这部分注释掉，减少运行时间
    {
        std::auto_ptr<MultiScalarField3D<T> > floatTags = copyConvert<int,T>(geometry, geometry.getBoundingBox());
        std::vector<T> isoLevels;
        isoLevels.push_back(0.5);
        typedef TriangleSet<T>::Triangle Triangle;
        std::vector<Triangle> triangles;
        Box3D domain = floatTags->getBoundingBox().enlarge(-1);
        domain.x0++;
        domain.x1--;
        isoSurfaceMarchingCube(triangles, *floatTags, isoLevels, domain);
        TriangleSet<T> set(triangles);
        std::string outDir = fNameOut + "/";
        set.writeBinarySTL(outDir + "porousMedium.stl");
    }
}

void porousMediaSetup(MultiBlockLattice3D<T,DESCRIPTOR>& lattice,
        OnLatticeBoundaryCondition3D<T,DESCRIPTOR>* boundaryCondition,
        MultiScalarField3D<int>& geometry, T deltaP)
{
    const plint nx = lattice.getNx();
    const plint ny = lattice.getNy();
    const plint nz = lattice.getNz();

    pcout << "Definition of inlet/outlet." << std::endl;
    Box3D inlet (0,0, 1,ny-2, 1,nz-2);
    boundaryCondition->addPressureBoundary0N(inlet, lattice);
    setBoundaryDensity(lattice, inlet, (T) 1.);

    Box3D outlet(nx-1,nx-1, 1,ny-2, 1,nz-2);
    boundaryCondition->addPressureBoundary0P(outlet, lattice);
    setBoundaryDensity(lattice, outlet, (T) 1. - deltaP*DESCRIPTOR<T>::invCs2);

    pcout << "Definition of the geometry." << std::endl;
    // Where "geometry" evaluates to 1, use bounce-back.
    defineDynamics(lattice, geometry, new BounceBack<T,DESCRIPTOR>(), 1);
    // Where "geometry" evaluates to 2, use no-dynamics (which does nothing).
    defineDynamics(lattice, geometry, new NoDynamics<T,DESCRIPTOR>(), 2);

    pcout << "Initilization of rho and u." << std::endl;
    initializeAtEquilibrium( lattice, lattice.getBoundingBox(), PressureGradient(deltaP, nx) );

    lattice.initialize();
    delete boundaryCondition;
}

void writeGifs(MultiBlockLattice3D<T,DESCRIPTOR>& lattice, plint iter)
{
    const plint nx = lattice.getNx();
    const plint ny = lattice.getNy();
    const plint nz = lattice.getNz();

    const plint imSize = 600;
    ImageWriter<T> imageWriter("leeloo");

    // Write velocity-norm at x=0.
    imageWriter.writeScaledGif(createFileName("ux_inlet", iter, 6),
            *computeVelocityNorm(lattice, Box3D(0,0, 0,ny-1, 0,nz-1)),
            imSize, imSize );

    // Write velocity-norm at x=nx/2.
    imageWriter.writeScaledGif(createFileName("ux_half", iter, 6),
            *computeVelocityNorm(lattice, Box3D(nx/2,nx/2, 0,ny-1, 0,nz-1)),
            imSize, imSize );
}

void writeVTK(MultiBlockLattice3D<T,DESCRIPTOR>& lattice, plint iter)
{
    VtkImageOutput3D<T> vtkOut(createFileName("vtk", iter, 6), 1.);
    vtkOut.writeData<float>(*computeVelocityNorm(lattice), "velocityNorm", 1.);
    vtkOut.writeData<3,float>(*computeVelocity(lattice), "velocity", 1.);
}

T computePermeability(MultiBlockLattice3D<T,DESCRIPTOR>& lattice, T nu, T deltaP, Box3D domain )
{
    pcout << "Computing the permeability." << std::endl;

    // Compute only the x-direction of the velocity (direction of the flow).
    plint xComponent = 0;
    plint nx = lattice.getNx();

    T meanU = computeAverage(*computeVelocityComponent(lattice, domain, xComponent));

    pcout << "Average velocity     = " << meanU                         << std::endl;
    pcout << "Lattice viscosity nu = " << nu                            << std::endl;
    pcout << "Grad P               = " << deltaP/(T)(nx-1)              << std::endl;
    pcout << "Permeability         = " << nu*meanU / (deltaP/(T)(nx-1)) << std::endl;

    return meanU;
}

int main(int argc, char **argv)
{
    plbInit(&argc, &argv);

    if (argc!=7) {
        pcout << "Error missing some input parameter\n";
        pcout << "The structure is :\n";
        pcout << "1. Input file name.\n";
        pcout << "2. Output directory name.\n";
        pcout << "3. number of cells in X direction.\n";
        pcout << "4. number of cells in Y direction.\n";
        pcout << "5. number of cells in Z direction.\n";
        pcout << "6. Delta P .\n";
        pcout << "Example: " << argv[0] << " twoSpheres.dat tmp/ 48 64 64 0.00005\n";
        exit (EXIT_FAILURE);
    }
    std::string fNameIn  = argv[1];
    std::string fNameOut = argv[2];

    const plint nx = atoi(argv[3]);
    const plint ny = atoi(argv[4]);
    const plint nz = atoi(argv[5]);
    const T deltaP = atof(argv[6]);

    global::directories().setOutputDir(fNameOut+"/");

    const T omega = 1.0;
    const T nu    = ((T)1/omega- (T)0.5)/DESCRIPTOR<T>::invCs2;

    pcout << "Creation of the lattice." << std::endl;
    
    //MultiBlockLattice3D类，是整个计算的计算主体，用于承载计算的全部操作，此处为在栈区新建对象
    //详情请见MultiBlockLattice系列文件
    MultiBlockLattice3D<T,DESCRIPTOR> lattice(nx,ny,nz, new BGKdynamics<T,DESCRIPTOR>(omega));
    
    // Switch off periodicity.
    //周期性边界条件的开关，用户手册里有详细说明
    lattice.periodicity().toggleAll(false);

    pcout << "Reading the geometry file." << std::endl;
    
    //MultiScalarField3D类，此处用于存储结构数据
    //详情请见multiDataField文件系列
    MultiScalarField3D<int> geometry(nx,ny,nz);
    readGeometry(fNameIn, fNameOut, geometry);

    pcout << "nu = " << nu << std::endl;
    pcout << "deltaP = " << deltaP << std::endl;
    pcout << "omega = " << omega << std::endl;
    pcout << "nx = " << lattice.getNx() << std::endl;
    pcout << "ny = " << lattice.getNy() << std::endl;
    pcout << "nz = " << lattice.getNz() << std::endl;
	
	//此处为调用上面设置好的porousMediaSetup函数
    porousMediaSetup(lattice, createLocalBoundaryCondition3D<T,DESCRIPTOR>(), geometry, deltaP);

    // The value-tracer is used to stop the simulation once is has converged.
    // 1st parameter:velocity
    // 2nd parameter:size
    // 3rd parameters:threshold
    // 1st and second parameters ae used for the length of the time average (size/velocity)
    //原注释写的很清楚，用于停止计算，设置的追踪函数
    util::ValueTracer<T> converge(1.0,1000.0,1.0e-4);

    pcout << "Simulation begins" << std::endl;
    plint iT=0;

    const plint maxT = 30000;
    for (;iT<maxT; ++iT) {
        if (iT % 20 == 0) {
            pcout << "Iteration " << iT << std::endl;
        }
        if (iT % 500 == 0 && iT>0) {
            writeGifs(lattice,iT);
        }
		
		//此处为所有计算的开始点，调用MultiBlockLattice3D类对象内的collideAndStream来开启演化计算
        lattice.collideAndStream();
        converge.takeValue(getStoredAverageEnergy(lattice),true);

        if (converge.hasConverged()) {
            break;
        }
    }

    pcout << "End of simulation at iteration " << iT << std::endl;

    pcout << "Permeability:" << std::endl << std::endl;
    computePermeability(lattice, nu, deltaP, lattice.getBoundingBox());
    pcout << std::endl;

    pcout << "Writing VTK file ..." << std::endl << std::endl;
    writeVTK(lattice,iT);
    pcout << "Finished!" << std::endl << std::endl;

    return 0;
}
```

#### 输入文件的形式
在不同的案例中结构相关的flag的值设置是不同的，此处的设置为，空隙0，固体边界1，固体实体2
可输入的文件格式，数据之间由空白字符相分割（空白字符：换行\n，制表\t，空格等），数据点数量等同于设置的计算域格点数量，也就是Nx* Ny *Nz，如果按照当前案例的输入来编写，循环的顺序是，x，y，z


```cpp
for(int x=0;x<nx;x++){
	for(int y=0;y<ny;y++){
		for(int z=0;z<nz;z++){
			//此处放你要输出的数据流
		}
	}
}
```
