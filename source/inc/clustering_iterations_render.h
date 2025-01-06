#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

#include <memory>
#include <thread>
#include <vector>

#include <IRender.h>
#include <commandqueue.h>
#include <resolver.h>
#include <resource.h>
#include <rootsignature.h>
#include <pipelinestate.h>
#include <config.h>

class CIterationsRender : public IRender {

public:

	CIterationsRender(Microsoft::WRL::ComPtr<ID3D12Device2> device, std::shared_ptr<CommandQueue> copyCQ, std::shared_ptr<CommandQueue> directCQ);
	~CIterationsRender();
	void LoadContent(double* updateFPS, D3D12_RESOURCE_DESC backBufferDescription);
	BOOL OnRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>, Microsoft::WRL::ComPtr<ID3D12Resource>, D3D12_CPU_DESCRIPTOR_HANDLE);
	BOOL OnUpdate(double elapsedTime, BYTE flag);
	void OnMouseWheel(float mouseWheelData);
	void OnMouseButtonLeftMove(WORD mX1, WORD mX2, WORD mY1, WORD mY2);
	void OnSpace();

private:

	struct Point {

	public:

		FLOAT x;
		FLOAT y;
		FLOAT z;

		Point(FLOAT x, FLOAT y, FLOAT z) : x(x), y(y), z(z) {}
		Point() : x(0.0f), y(0.0f), z(0.0f) {}
		Point(const Point& point) {

			x = point.x;
			y = point.y;
			z = point.z;

		}
		Point& operator=(const Point& point) {

			x = point.x;
			y = point.y;
			z = point.z;

			return *this;

		}

		BOOL operator==(const Point& point) {

			if (abs(x - point.x) >= 0.000000100f) return FALSE;
			if (abs(y - point.y) >= 0.000000100f) return FALSE;
			if (abs(z - point.z) >= 0.000000100f) return FALSE;
			return TRUE;

		}

		Point& operator*=(const float& scalar) {

			this->x *= scalar;
			this->y *= scalar;
			this->z *= scalar;

			return *this;

		}

		Point& operator-=(const float& scalar) {

			this->x -= scalar;
			this->y -= scalar;
			this->z -= scalar;

			return *this;

		}
		Point& operator-=(const Point& point) {

			this->x -= point.x;
			this->y -= point.y;
			this->z -= point.z;

			return *this;

		}

		Point& operator+=(const float& scalar) {

			this->x += scalar;
			this->y += scalar;
			this->z += scalar;

			return *this;

		}
		Point& operator+=(const Point& point) {

			this->x += point.x;
			this->y += point.y;
			this->z += point.z;

			return *this;

		}

	};

	struct Edge {

		Point a;
		Point b;

		Edge(Point a, Point b) : a(a), b(b) {}
		Edge() : a(Point()), b(Point()) {}
		Edge(const Edge& edge) {

			a = edge.a;
			b = edge.b;

		}
		Edge& operator=(const Edge& edge) {

			a = edge.a;
			b = edge.b;

			return *this;

		}

		BOOL operator==(const Edge& edge) {

			if (a == edge.a && b == edge.b) return TRUE;
			if (a == edge.b && b == edge.a) return TRUE;
			return FALSE;
		}

	};

	struct Triangle {

	public:

		Point a;
		Point b;
		Point c;

		Triangle(Point a, Point b, Point c) : a(a), b(b), c(c) {}
		Triangle() : a(Point()), b(Point()), c(Point()) {}
		BOOL operator==(const Triangle& triangle) {

			if (a == triangle.a && b == triangle.b && c == triangle.c) return TRUE;
			if (a == triangle.a && b == triangle.c && c == triangle.b) return TRUE;
			if (a == triangle.b && b == triangle.a && c == triangle.c) return TRUE;
			if (a == triangle.b && b == triangle.c && c == triangle.a) return TRUE;
			if (a == triangle.c && b == triangle.b && c == triangle.a) return TRUE;
			if (a == triangle.c && b == triangle.a && c == triangle.b) return TRUE;
			return FALSE;

		}

	};

	struct VoronoiEdge : Edge {

		Triangle triangle;

		VoronoiEdge(Point a, Point b) : Edge(a, b), triangle(Triangle()) {}

	};

	struct ColorEdge : Edge {

		Point color;

		ColorEdge(Point a, Point b, Point color) : Edge(a, b), color(color) {}

	};

	struct ColorTriangle : Triangle {

		Point color;

		ColorTriangle(Point a, Point b, Point c, Point color) : Triangle(a, b, c), color(color) {}

	};

	struct NormalColorTriangle : ColorTriangle {

		Point normal;

		NormalColorTriangle(Point a, Point b, Point c, Point color, Point normal) : ColorTriangle(a, b, c, color), normal(normal) {}

	};

	struct Tetrahedron {

		Point a;
		Point b;
		Point c;
		Point d;
		Point circumcenter;
		FLOAT circumradius;

	};

	struct EdgeIndex {

		UINT a;
		UINT b;

		EdgeIndex() : a(0), b(0) {}
		EdgeIndex(UINT a, UINT b) : a(a), b(b) {}

	};

	struct Plane {

		FLOAT xCoef;
		FLOAT yCoef;
		FLOAT zCoef;
		FLOAT kCoef;

		Plane(FLOAT xCoef, FLOAT yCoef, FLOAT zCoef, FLOAT kCoef) : xCoef(xCoef), yCoef(yCoef), zCoef(zCoef), kCoef(kCoef) {}
		Plane() : xCoef(0.0f), yCoef(0.0f), zCoef(0.0f), kCoef(0.0f) {}

	};

	typedef Plane PlaneNORMAL;

	//K-means clustering and voronoi diagram

	void ResetCentroids();

	void StartClusterAndVoronoi();
	void EndClusterAndVoronoi();

	void ClusterAndVoronoi();

	void CalculateCircumsphere(UINT i);
	void CalculateCircumsphere(Tetrahedron* tetrahedron);
	BOOL IsFace(Point* a, Point* b, Point* c, Tetrahedron* tetrahedron);
	BOOL IsEdge(Point* a, Point* b, Tetrahedron* tetrahedron);
	BOOL LinePlaneIntersection(Point* lineA, Point* lineB, FLOAT planeXCoef, FLOAT planeYCoef, FLOAT planeZCoef, FLOAT planeKCoef, Point* intersection);

	//DirectX

	void LoadShaders();

	void InitVertexBufferPointListPixelPosition();
	void UploadVertexBufferPointListPixelPosition();

	void InitVertexBufferTriangleListVoronoiDiagram();
	void UploadVertexBufferTriangleListVoronoiDiagram();

	void InitRootSignatureNoLighting();
	void InitRootSignatureLighting();
	void InitRootSignatureDownsampling();

	void InitPipelineStatePointListNoLighting();
	void InitPipelineStateTriangleListPhongLighting();
	void InitPipelineStateDownsampling();

	void InitDepthBuffer();

	void InitOriginalVideoFrameBuffer();
	void UploadOriginalVideoFrameBuffer();

	void InitQuantizedVideoFrameBuffer();
	void FillQuantizedVideoFrameBuffer();

	void InitDowsamplingDescriptorHeaps();



	void DownsampleRender(Microsoft::WRL::ComPtr<ID3D12Resource> resource);
	void CopyToBackBuffer(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer,
		Microsoft::WRL::ComPtr<ID3D12Resource> resource, UINT subresource, UINT offsetX, UINT offsetY, UINT offsetZ);



	DirectX::XMVECTOR GetUnitVector(WORD mX, WORD mY);



	std::thread m_clusterAndVoronoiThread;

	Point m_centroids[COMPUTE_SHADER_KC_CENTROID_COUNT];

	std::vector<Tetrahedron> m_triangulation = {};
	std::vector<Tetrahedron*> m_badTriangulation = {};
	std::vector<UINT> m_badTriangulationIndex = {};
	std::vector<Triangle> m_polyhedron = {};

	std::vector<Edge> m_delaunayEdges = {};
	std::vector<EdgeIndex> m_delaunayEdgesIndex = {};
	std::vector<Tetrahedron> m_centroidTetrahedrons = {};

	std::vector<Point> m_voronoiVertices = {};
	std::vector<VoronoiEdge> m_voronoiEdges = {};
	std::vector<Edge> m_voronoiFace = {};
	std::vector<Edge> m_voronoiFaceOrdered = {};
	std::vector<ColorEdge> m_voronoiColoredEdges = {};
	std::vector<ColorTriangle> m_voronoiColoredTriangles = {};
	std::vector<Edge> m_voronoiFaces = {};
	std::vector<UINT> m_voronoiFacesCount = {};

	std::vector<std::vector<Edge>> m_voronoiCells[COMPUTE_SHADER_KC_CENTROID_COUNT] = {};
	std::vector<std::vector<Edge>> m_separateVoronoiFaces = {};
	std::vector<UINT> m_pointedVoronoiCells[COMPUTE_SHADER_KC_CENTROID_COUNT] = {};
	std::vector<Plane> m_separateVoronoiFacePlanes = {};
	std::vector<Point> m_separateVoronoiFaceColors = {};

	std::vector<Point> m_cubeCrossSection = {};
	std::vector<FLOAT> m_cubeCrossSectionCentroidAngle = {};
	std::vector<Point> m_cubeCrossSectionOrdered = {};
	std::vector<std::vector<Point>> m_separateVoronoiFacesCulledOrdered = {};

	std::vector<Edge> m_unitCubeEdges = {};
	std::vector<Point> m_unitCubeVertices = {};
	std::vector<ColorTriangle> m_clippedVoronoiTriangles = {};

	std::vector<UINT> m_culledVoronoiFacesIndex = {};
	std::vector<ColorTriangle> m_clippedVoronoiCellsTriangles = {};

	std::vector<Point> m_unitCubeFace1Points = {};
	std::vector<Point> m_unitCubeFace2Points = {};
	std::vector<Point> m_unitCubeFace3Points = {};
	std::vector<Point> m_unitCubeFace4Points = {};
	std::vector<Point> m_unitCubeFace5Points = {};
	std::vector<Point> m_unitCubeFace6Points = {};
	std::vector<FLOAT> m_unitCubeFaceCentroidAngle = {};
	std::vector<Point> m_unitCubeFaceOrdered = {};

	std::vector<PlaneNORMAL> m_separateVoronoiFaceNormals = {};
	std::vector<NormalColorTriangle> m_clippedVoronoiTrianglesFaceNormals = {};

	std::vector<ColorEdge> m_voronoiEdgesUnitCube = {};
	std::vector<ColorTriangle> m_clippedVoronoiCellsBackCulledTriangles = {};
	std::vector<NormalColorTriangle> m_clippedVoronoiCellsBackCulledTrianglesNormals = {};

	UINT m_preClipFaceCount = 0;

	UINT m_voronoiDiagramTriangleCount = 0;

	UINT m_kmeansClusteringIteration;

	std::unique_ptr<Resolver> m_resolver;

	UINT64 m_videoWidth;
	UINT64 m_videoHeight;
	UINT64 m_videoStride;

	IMFSample* m_videoSample;

	DirectX::XMVECTOR m_cameraPosition;
	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_worldViewProjectionMatrix;

	BOOL m_isLoaded;

	std::shared_ptr<CommandQueue> m_copyCQ;
	std::shared_ptr<CommandQueue> m_directCQ;

	//DirectX

	Microsoft::WRL::ComPtr<ID3D12Device2> m_device;

	std::unique_ptr<Resource> m_vertexBufferPointListPixelPosition;
	std::unique_ptr<Resource> m_vertexBufferTriangleListVoronoiDiagram;

	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferViewPointListPixelPosition;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferViewTriangleListVoronoiDiagram;

	std::unique_ptr<RootSignature> m_rootSignatureNoLighting;
	std::unique_ptr<RootSignature> m_rootSignaturePhongLighting;
	std::unique_ptr<RootSignature> m_rootSignatureDownsampling;
	D3D12_FEATURE_DATA_ROOT_SIGNATURE m_rootSignatureVersionSupport;

	std::unique_ptr<PipelineState> m_pipelineStatePoint;
	std::unique_ptr<PipelineState> m_pipelineStateTriangle;
	std::unique_ptr<PipelineState> m_c_pipelineStateDownsampling;

	Microsoft::WRL::ComPtr<ID3DBlob> m_vs_pointListNoLighting;
	Microsoft::WRL::ComPtr<ID3DBlob> m_vs_triangleListPhongLighting;
	
	Microsoft::WRL::ComPtr<ID3DBlob> m_ps_noLighting;
	Microsoft::WRL::ComPtr<ID3DBlob> m_ps_phongLighting;

	Microsoft::WRL::ComPtr<ID3DBlob> m_cs_downsampling;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;
	std::unique_ptr<Resource> m_depthBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dh_sampler_downsampling;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dh_srv_uav_cbv_downsampling;
	std::unique_ptr<Resource> m_originalVideoFrame;
	std::unique_ptr<Resource> m_quantizedVideoFrame;

	D3D12_RECT m_scissorRectangle;
	D3D12_VIEWPORT m_viewport;

};