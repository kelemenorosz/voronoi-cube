#include <subspace_render.h>

#include <d3dx12.h>

//Link DirectX libraries
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "D3DCompiler.lib")

//Link Media Foundation libraries
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfreadwrite.lib")

//Media Foundation includes
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>

//DirectX includes
#include <d3d12.h>
#include <d3dcompiler.h>
#include <directxmath.h>

//Project external includes
#include <ctime>

//Project internal includes
#include <helper.h>
#include <config.h>
#include <commandqueue.h>
#include <resource.h>

namespace mWRL = Microsoft::WRL;

void SubspaceRender::LoadContent(double* updateFPS, D3D12_RESOURCE_DESC backBufferDescription) {

	//Initialize resolver for video file
	m_resolver = std::make_unique<Resolver>(INPUT_VIDEO_FILE_PATH);
	
	//Query for frame width, stride and height
	UINT64 videoFrameWidth = 0;
	UINT64 videoFrameStride = 0;
	UINT64 videoFrameHeight = 0;
	m_resolver->GetVideoFrameMetrics(&videoFrameWidth, &videoFrameHeight, &videoFrameStride);

	//Skip first 60 frames
	for (UINT i = 0; i < 60; ++i) {

		IMFSample* videoSample = nullptr;
		m_resolver->StartGetSample();
		m_resolver->EndGetSample(&videoSample);
		SafeRelease(&videoSample);

	}

	

	//Create COMPUTE_SHADER_KC_CENTROID_COUNT number of points
	for (UINT i = 0; i < COMPUTE_SHADER_KC_CENTROID_COUNT; ++i) {

		m_centroids[i].x = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
		m_centroids[i].y = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
		m_centroids[i].z = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;

	}

	/*FLOAT randomNumber = 0.0f;
	randomNumber = 1.0f / static_cast<FLOAT>(COMPUTE_SHADER_KC_CENTROID_COUNT + 1);

	for (UINT i = 1; i < COMPUTE_SHADER_KC_CENTROID_COUNT + 1; ++i) {

		m_centroids[i - 1].x = randomNumber * static_cast<FLOAT>(i);
		m_centroids[i - 1].y = randomNumber * static_cast<FLOAT>(i);
		m_centroids[i - 1].z = randomNumber * static_cast<FLOAT>(i);
	}*/

	//Specific centroid input
	/*m_centroids[0].x = 0.254066586f;
	m_centroids[0].y = 0.332499146f;
	m_centroids[0].z = 0.467238367f;

	m_centroids[1].x = 0.257118434f;
	m_centroids[1].y = 0.892544329f;
	m_centroids[1].z = 0.709067047f;*/

	//Specific centroid input
	/*m_centroids[0].x = 0.754112363f;
	m_centroids[0].y = 0.150090024f;
	m_centroids[0].z = 0.597643971f;

	m_centroids[1].x = 0.507980585f;
	m_centroids[1].y = 0.0390636921f;
	m_centroids[1].z = 0.0964384899f;

	m_centroids[2].x = 0.161351353f;
	m_centroids[2].y = 0.188634902f;
	m_centroids[2].z = 0.141453296f;*/

	//Specific centroid input
	/*m_centroids[0].x = 0.844416618f;
	m_centroids[0].y = 0.292214721f;
	m_centroids[0].z = 0.890224934f;

	m_centroids[1].x = 0.599536121f;
	m_centroids[1].y = 0.478896439f;
	m_centroids[1].z = 0.767601550f;

	m_centroids[2].x = 0.774926007f;
	m_centroids[2].y = 0.671254635f;
	m_centroids[2].z = 0.111636706f;

	m_centroids[3].x = 0.249336228f;
	m_centroids[3].y = 0.417950988f;
	m_centroids[3].z = 0.791650116f;*/

	for (UINT i = 0; i < COMPUTE_SHADER_KC_CENTROID_COUNT; ++i) {

		m_centroidColors[i].x = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
		m_centroidColors[i].y = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
		m_centroidColors[i].z = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;

	}

	Tetrahedron superTetrahedron = {};

	//Divide the cube into 5 tetrahedrons
	{

		//Tetrahedron 1.
		//superTetrahedron.a = Point(0.0f, 0.0f, 0.0f);
		//superTetrahedron.b = Point(0.0f, 1.0f, 1.0f);
		//superTetrahedron.c = Point(1.0f, 1.0f, 0.0f);
		//superTetrahedron.d = Point(1.0f, 0.0f, 1.0f);
		//m_triangulation.push_back(superTetrahedron);

		////Tetrahedron 2.
		//superTetrahedron.a = Point(0.0f, 0.0f, 0.0f);
		//superTetrahedron.b = Point(0.0f, 1.0f, 0.0f);
		//superTetrahedron.c = Point(1.0f, 1.0f, 0.0f);
		//superTetrahedron.d = Point(0.0f, 1.0f, 1.0f);
		//m_triangulation.push_back(superTetrahedron);

		////Tetrahedron 3.
		//superTetrahedron.a = Point(1.0f, 1.0f, 1.0f);
		//superTetrahedron.b = Point(1.0f, 0.0f, 1.0f);
		//superTetrahedron.c = Point(1.0f, 1.0f, 0.0f);
		//superTetrahedron.d = Point(0.0f, 1.0f, 1.0f);
		//m_triangulation.push_back(superTetrahedron);

		////Tetrahedron 4.
		//superTetrahedron.a = Point(1.0f, 0.0f, 0.0f);
		//superTetrahedron.b = Point(0.0f, 0.0f, 0.0f);
		//superTetrahedron.c = Point(1.0f, 1.0f, 0.0f);
		//superTetrahedron.d = Point(1.0f, 0.0f, 1.0f);
		//m_triangulation.push_back(superTetrahedron);

		////Tetrahedron 5.
		//superTetrahedron.a = Point(0.0f, 0.0f, 1.0f);
		//superTetrahedron.b = Point(0.0f, 0.0f, 0.0f);
		//superTetrahedron.c = Point(1.0f, 0.0f, 1.0f);
		//superTetrahedron.d = Point(0.0f, 1.0f, 1.0f);
		//m_triangulation.push_back(superTetrahedron);
	
	}

	//Calculate super tetrahedrons' circumsphere
	/*for (UINT i = 0; i < m_triangulation.size(); ++i) {

		this->CalculateCircumsphere(i);

	}*/

	//Super tetrahedron whose insphere is the circumshpere of the cube
	Tetrahedron cubeTetrahedron = {};
	cubeTetrahedron.a = Point(0.0f, 0.0f, 0.0f);
	cubeTetrahedron.b = Point(0.0f, 1.0f, 0.0f);
	cubeTetrahedron.c = Point(1.0f, 0.0f, 0.0f);
	cubeTetrahedron.d = Point(0.0f, 0.0f, 1.0f);
	this->CalculateCircumsphere(&cubeTetrahedron);

	FLOAT superTetrahedronEdgeLength = 2.0f * sqrtf(6.0f) * cubeTetrahedron.circumradius;
	superTetrahedron.a = Point((1.0f * superTetrahedronEdgeLength) / 2.0f + 0.5f, (1.0f * superTetrahedronEdgeLength) / 2.0f + 0.5f, (1.0f * superTetrahedronEdgeLength) / 2.0f + 0.5f);
	superTetrahedron.b = Point((1.0f * superTetrahedronEdgeLength) / 2.0f + 0.5f, (-1.0f * superTetrahedronEdgeLength) / 2.0f + 0.5f, (-1.0f * superTetrahedronEdgeLength) / 2.0f + 0.5f);
	superTetrahedron.c = Point((-1.0f * superTetrahedronEdgeLength) / 2.0f + 0.5f, (1.0f * superTetrahedronEdgeLength) / 2.0f + 0.5f, (-1.0f * superTetrahedronEdgeLength) / 2.0f + 0.5f);
	superTetrahedron.d = Point((-1.0f * superTetrahedronEdgeLength) / 2.0f + 0.5f, (-1.0f * superTetrahedronEdgeLength) / 2.0f + 0.5f, (1.0f * superTetrahedronEdgeLength) / 2.0f + 0.5f);
	m_triangulation.push_back(superTetrahedron);
	this->CalculateCircumsphere(UINT(0));

	FLOAT circumcenterDistance = 0.0f;
	FLOAT distanceX = 0.0f;
	FLOAT distanceY = 0.0f;
	FLOAT distanceZ = 0.0f;
	FLOAT circumradiusSq = 0.0f;

	BOOL isFace = FALSE;
	Point triangleA = Point();
	Point triangleB = Point();
	Point triangleC = Point();

	Tetrahedron newTetrahedron = {};

	UINT numDuplicates = 0;
	BOOL isDuplicate = TRUE;
	UINT duplicateIndexA = 0;
	UINT duplicateIndexB = 0;

	//Bowyer-Watson
	for (UINT i = 0; i < COMPUTE_SHADER_KC_CENTROID_COUNT; ++i) {

		//Clear m_badTriangulation, m_badTriangulationIndex and m_polyhedron
		m_badTriangulation.clear();
		m_badTriangulationIndex.clear();
		m_polyhedron.clear();

		//Fill m_badTriangulation with tetrahedrons whose circumsphere contains the i-th centroid point
		for (UINT j = 0; j < m_triangulation.size(); ++j) {

			distanceX = m_triangulation[j].circumcenter.x - m_centroids[i].x;
			distanceY = m_triangulation[j].circumcenter.y - m_centroids[i].y;
			distanceZ = m_triangulation[j].circumcenter.z - m_centroids[i].z;
			circumcenterDistance = distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ;
			circumradiusSq = m_triangulation[j].circumradius * m_triangulation[j].circumradius;

			if (circumcenterDistance < circumradiusSq) {

				m_badTriangulation.push_back(&m_triangulation[j]);
				m_badTriangulationIndex.push_back(j);

			}

		}

		//Construct the polyhedron around the i-th centroid
		{

			//for (UINT j = 0; j < m_badTriangulation.size(); ++j) {

			//	//Triangle 1.
			//	triangleA = m_badTriangulation[j]->a;
			//	triangleB = m_badTriangulation[j]->b;
			//	triangleC = m_badTriangulation[j]->c;

			//	isFace = FALSE;
			//	for (UINT k = 0; k < m_badTriangulation.size(); ++k) {

			//		if (j != k) {

			//			isFace = isFace || this->IsFace(&triangleA, &triangleB, &triangleC, m_badTriangulation[k]);

			//		}

			//	}
			//	if (!isFace) m_polyhedron.push_back(Triangle(triangleA, triangleB, triangleC));

			//	//Triangle 2.
			//	triangleA = m_badTriangulation[j]->a;
			//	triangleB = m_badTriangulation[j]->c;
			//	triangleC = m_badTriangulation[j]->d;

			//	isFace = FALSE;
			//	for (UINT k = 0; k < m_badTriangulation.size(); ++k) {

			//		if (j != k) {

			//			isFace = isFace || this->IsFace(&triangleA, &triangleB, &triangleC, m_badTriangulation[k]);

			//		}

			//	}
			//	if (!isFace) m_polyhedron.push_back(Triangle(triangleA, triangleB, triangleC));

			//	//Triangle 3.
			//	triangleA = m_badTriangulation[j]->a;
			//	triangleB = m_badTriangulation[j]->b;
			//	triangleC = m_badTriangulation[j]->d;

			//	isFace = FALSE;
			//	for (UINT k = 0; k < m_badTriangulation.size(); ++k) {

			//		if (j != k) {

			//			isFace = isFace || this->IsFace(&triangleA, &triangleB, &triangleC, m_badTriangulation[k]);

			//		}

			//	}
			//	if (!isFace) m_polyhedron.push_back(Triangle(triangleA, triangleB, triangleC));

			//	//Triangle 4.
			//	triangleA = m_badTriangulation[j]->b;
			//	triangleB = m_badTriangulation[j]->c;
			//	triangleC = m_badTriangulation[j]->d;

			//	isFace = FALSE;
			//	for (UINT k = 0; k < m_badTriangulation.size(); ++k) {

			//		if (j != k) {

			//			isFace = isFace || this->IsFace(&triangleA, &triangleB, &triangleC, m_badTriangulation[k]);

			//		}

			//	}
			//	if (!isFace) m_polyhedron.push_back(Triangle(triangleA, triangleB, triangleC));


			//}
		
		}

		//Construct the polyhedron around the i-th centroid - revision
		for (UINT j = 0; j < m_badTriangulation.size(); ++j) {

			//Triangle 1.
			isFace = FALSE;
			for (UINT k = 0; k < m_badTriangulation.size(); ++k) {

				if (j != k) {

					if (this->IsFace(&m_badTriangulation[j]->a, &m_badTriangulation[j]->b, &m_badTriangulation[j]->c, m_badTriangulation[k])) isFace = TRUE;

				}
				
			}
			if (!isFace) {

				m_polyhedron.emplace_back(m_badTriangulation[j]->a, m_badTriangulation[j]->b, m_badTriangulation[j]->c);

			}

			//Triangle 2.
			isFace = FALSE;
			for (UINT k = 0; k < m_badTriangulation.size(); ++k) {

				if (j != k) {

					if (this->IsFace(&m_badTriangulation[j]->a, &m_badTriangulation[j]->b, &m_badTriangulation[j]->d, m_badTriangulation[k])) isFace = TRUE;

				}
				
			}

			if (!isFace) {

				m_polyhedron.emplace_back(m_badTriangulation[j]->a, m_badTriangulation[j]->b, m_badTriangulation[j]->d);

			}

			//Triangle 3.
			isFace = FALSE;
			for (UINT k = 0; k < m_badTriangulation.size(); ++k) {

				if (j != k) {

					if (this->IsFace(&m_badTriangulation[j]->a, &m_badTriangulation[j]->c, &m_badTriangulation[j]->d, m_badTriangulation[k])) isFace = TRUE;

				}
				
			}
			if (!isFace) {

				m_polyhedron.emplace_back(m_badTriangulation[j]->a, m_badTriangulation[j]->c, m_badTriangulation[j]->d);

			}

			//Triangle 4.
			isFace = FALSE;
			for (UINT k = 0; k < m_badTriangulation.size(); ++k) {

				if (j != k) {

					if (this->IsFace(&m_badTriangulation[j]->b, &m_badTriangulation[j]->c, &m_badTriangulation[j]->d, m_badTriangulation[k])) isFace = TRUE;

				}
				
			}
			if (!isFace) {

				m_polyhedron.emplace_back(m_badTriangulation[j]->b, m_badTriangulation[j]->c, m_badTriangulation[j]->d);

			}

		}

		//Remove m_badTriangulation tetrahedrons from m_triangulation
		for (INT j = static_cast<INT>(m_badTriangulation.size()) - 1; j >= 0; --j) {

			m_triangulation.erase(m_triangulation.begin() + m_badTriangulationIndex[j]);

		}

		//Retriangulate the polyhedron around the i-th centroid
		for (UINT j = 0; j < m_polyhedron.size(); ++j) {

			newTetrahedron.a = m_polyhedron[j].a;
			newTetrahedron.b = m_polyhedron[j].b;
			newTetrahedron.c = m_polyhedron[j].c;
			newTetrahedron.d = Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z);
			m_triangulation.push_back(newTetrahedron);

			this->CalculateCircumsphere(static_cast<UINT>(m_triangulation.size()) - 1);

		}

		numDuplicates = 0;
		isDuplicate = TRUE;
		duplicateIndexA = 0;
		duplicateIndexB = 0;
		//Check for Bowyer-Watson triangulation duplicates
		for (UINT i = 0; i < m_triangulation.size(); ++i) {

			for (UINT j = i + 1; j < m_triangulation.size(); ++j) {

				isDuplicate = TRUE;
				if (!this->IsFace(&m_triangulation[i].a, &m_triangulation[i].b, &m_triangulation[i].c, &m_triangulation[j])) isDuplicate = FALSE;
				if (!this->IsFace(&m_triangulation[i].a, &m_triangulation[i].b, &m_triangulation[i].d, &m_triangulation[j])) isDuplicate = FALSE;
				if (!this->IsFace(&m_triangulation[i].a, &m_triangulation[i].c, &m_triangulation[i].d, &m_triangulation[j])) isDuplicate = FALSE;
				if (!this->IsFace(&m_triangulation[i].b, &m_triangulation[i].c, &m_triangulation[i].d, &m_triangulation[j])) isDuplicate = FALSE;

				if (isDuplicate) {

					++numDuplicates;
					duplicateIndexA = i;
					duplicateIndexB = j;

				}

			}

		}

	}

	//Delaunay

	Edge edgeA = {};
	Edge edgeB = {};
	Edge edgeC = {};
	Edge edgeD = {};
	Edge edgeE = {};
	Edge edgeF = {};

	BOOL isEdgeAAlreadyAdded = FALSE;
	BOOL isEdgeBAlreadyAdded = FALSE;
	BOOL isEdgeCAlreadyAdded = FALSE;
	BOOL isEdgeDAlreadyAdded = FALSE;
	BOOL isEdgeEAlreadyAdded = FALSE;
	BOOL isEdgeFAlreadyAdded = FALSE;

	//Find delaunay edges
	{

		//for (UINT i = 0; i < m_triangulation.size(); ++i) {

		//	//Edge 1.
		//	edgeA.a = m_triangulation[i].a;
		//	edgeA.b = m_triangulation[i].b;

		//	//Edge 2.
		//	edgeB.a = m_triangulation[i].a;
		//	edgeB.b = m_triangulation[i].c;

		//	//Edge 3.
		//	edgeC.a = m_triangulation[i].a;
		//	edgeC.b = m_triangulation[i].d;

		//	//Edge 4.
		//	edgeD.a = m_triangulation[i].b;
		//	edgeD.b = m_triangulation[i].c;

		//	//Edge 5.
		//	edgeE.a = m_triangulation[i].b;
		//	edgeE.b = m_triangulation[i].d;

		//	//Edge 6.
		//	edgeF.a = m_triangulation[i].c;
		//	edgeF.b = m_triangulation[i].d;

		//	isEdgeAAlreadyAdded = FALSE;
		//	isEdgeBAlreadyAdded = FALSE;
		//	isEdgeCAlreadyAdded = FALSE;
		//	isEdgeDAlreadyAdded = FALSE;
		//	isEdgeEAlreadyAdded = FALSE;
		//	isEdgeFAlreadyAdded = FALSE;
		//	for (UINT j = 0; j < m_delaunayEdges.size(); ++j) {

		//		if (edgeA == m_delaunayEdges[j]) isEdgeAAlreadyAdded = TRUE;
		//		if (edgeB == m_delaunayEdges[j]) isEdgeBAlreadyAdded = TRUE;
		//		if (edgeC == m_delaunayEdges[j]) isEdgeCAlreadyAdded = TRUE;
		//		if (edgeD == m_delaunayEdges[j]) isEdgeDAlreadyAdded = TRUE;
		//		if (edgeE == m_delaunayEdges[j]) isEdgeEAlreadyAdded = TRUE;
		//		if (edgeF == m_delaunayEdges[j]) isEdgeFAlreadyAdded = TRUE;

		//	}

		//	if (isEdgeAAlreadyAdded == FALSE) m_delaunayEdges.push_back(edgeA);
		//	if (isEdgeBAlreadyAdded == FALSE) m_delaunayEdges.push_back(edgeB);
		//	if (isEdgeCAlreadyAdded == FALSE) m_delaunayEdges.push_back(edgeC);
		//	if (isEdgeDAlreadyAdded == FALSE) m_delaunayEdges.push_back(edgeD);
		//	if (isEdgeEAlreadyAdded == FALSE) m_delaunayEdges.push_back(edgeE);
		//	if (isEdgeFAlreadyAdded == FALSE) m_delaunayEdges.push_back(edgeF);

		//}

	}

	BOOL isDelaunayEdge = FALSE;

	//Find delaunay edges - only connected centroids
	for (UINT i = 0; i < COMPUTE_SHADER_KC_CENTROID_COUNT; ++i) {

		for (UINT j = i + 1; j < COMPUTE_SHADER_KC_CENTROID_COUNT; ++j) {

			isDelaunayEdge = FALSE;
			for (UINT k = 0; k < m_triangulation.size(); ++k) {

				if (this->IsEdge(&m_centroids[i], &m_centroids[j], &m_triangulation[k])) isDelaunayEdge = TRUE;

			}
			if (isDelaunayEdge) {

				m_delaunayEdges.emplace_back(Point(m_centroids[i]), Point(m_centroids[j]));
				m_delaunayEdgesIndex.emplace_back(i, j);

			}

		}

	}

	BOOL isCentroidA = FALSE;
	BOOL isCentroidB = FALSE;
	BOOL isCentroidC = FALSE;
	BOOL isCentroidD = FALSE;

	//Remove tetrahedrons with super vertices from m_triangulation
	/*for (INT i = static_cast<INT>(m_triangulation.size()) - 1; i >= 0; --i) {

		isCentroidA = FALSE;
		isCentroidB = FALSE;
		isCentroidC = FALSE;
		isCentroidD = FALSE;
		
		for (UINT j = 0; j < COMPUTE_SHADER_KC_CENTROID_COUNT; ++j) {

			if (m_triangulation[i].a == m_centroids[j]) isCentroidA = TRUE;
			if (m_triangulation[i].b == m_centroids[j]) isCentroidB = TRUE;
			if (m_triangulation[i].c == m_centroids[j]) isCentroidC = TRUE;
			if (m_triangulation[i].d == m_centroids[j]) isCentroidD = TRUE;

		}

		if (isCentroidA == FALSE || isCentroidB == FALSE || isCentroidC == FALSE || isCentroidD == FALSE) m_triangulation.erase(m_triangulation.begin() + i);

	}*/

	//Voronoi

	//Find voronoi vertices - delauney tetrahedron circumcenters
	for (UINT i = 0; i < m_triangulation.size(); ++i) {

		m_voronoiVertices.emplace_back(m_triangulation[i].circumcenter);

	}

	//Find voronoi edges - connected delauney tetrahedron circumcenters
	{

		//for (UINT i = 0; i < m_triangulation.size(); ++i) {

		//	//Triangle 1.
		//	triangleA = m_triangulation[i].a;
		//	triangleB = m_triangulation[i].b;
		//	triangleC = m_triangulation[i].c;
		//	for (UINT j = 0; j < m_triangulation.size(); ++j) {

		//		if (i != j) {

		//			if (this->IsFace(&triangleA, &triangleB, &triangleC, &m_triangulation[j])) {

		//				m_voronoiEdges.emplace_back(m_triangulation[i].circumcenter, m_triangulation[j].circumcenter);
		//				m_voronoiEdges[m_voronoiEdges.size() - 1].triangle = Triangle(triangleA, triangleB, triangleC);

		//			}

		//		}

		//	}

		//	//Triangle 2.
		//	triangleA = m_triangulation[i].a;
		//	triangleB = m_triangulation[i].b;
		//	triangleC = m_triangulation[i].d;
		//	for (UINT j = 0; j < m_triangulation.size(); ++j) {

		//		if (i != j) {

		//			if (this->IsFace(&triangleA, &triangleB, &triangleC, &m_triangulation[j])) {

		//				m_voronoiEdges.emplace_back(m_triangulation[i].circumcenter, m_triangulation[j].circumcenter);
		//				m_voronoiEdges[m_voronoiEdges.size() - 1].triangle = Triangle(triangleA, triangleB, triangleC);

		//			}

		//		}

		//	}

		//	//Triangle 3.
		//	triangleA = m_triangulation[i].a;
		//	triangleB = m_triangulation[i].c;
		//	triangleC = m_triangulation[i].d;
		//	for (UINT j = 0; j < m_triangulation.size(); ++j) {

		//		if (i != j) {

		//			if (this->IsFace(&triangleA, &triangleB, &triangleC, &m_triangulation[j])) {

		//				m_voronoiEdges.emplace_back(m_triangulation[i].circumcenter, m_triangulation[j].circumcenter);
		//				m_voronoiEdges[m_voronoiEdges.size() - 1].triangle = Triangle(triangleA, triangleB, triangleC);

		//			}

		//		}

		//	}

		//	//Triangle 4.
		//	triangleA = m_triangulation[i].b;
		//	triangleB = m_triangulation[i].c;
		//	triangleC = m_triangulation[i].d;
		//	for (UINT j = 0; j < m_triangulation.size(); ++j) {

		//		if (i != j) {

		//			if (this->IsFace(&triangleA, &triangleB, &triangleC, &m_triangulation[j])) {

		//				m_voronoiEdges.emplace_back(m_triangulation[i].circumcenter, m_triangulation[j].circumcenter);
		//				m_voronoiEdges[m_voronoiEdges.size() - 1].triangle = Triangle(triangleA, triangleB, triangleC);

		//			}

		//		}

		//	}

		//}

	}

	//How many shared circumcenters
	UINT sharedCircumcenters = 0;
	for (UINT i = 0; i < m_triangulation.size(); ++i) {

		for (UINT j = i + 1; j < m_triangulation.size(); ++j) {

			if (m_triangulation[i].circumcenter == m_triangulation[j].circumcenter) ++sharedCircumcenters;

		}

	}

	//Find voronoi edges - connected delaunay tetrahedron circumcenters - only one copy of the edge
	for (UINT i = 0; i < m_triangulation.size(); ++i) {

		for (UINT j = i + 1; j < m_triangulation.size(); ++j) {

			if (this->IsFace(&m_triangulation[i].a, &m_triangulation[i].b, &m_triangulation[i].c, &m_triangulation[j])) {
			
				m_voronoiEdges.emplace_back(m_triangulation[i].circumcenter, m_triangulation[j].circumcenter);
				m_voronoiEdges[m_voronoiEdges.size() - 1].triangle = Triangle(m_triangulation[i].a, m_triangulation[i].b, m_triangulation[i].c);
			
			}
			
			if (this->IsFace(&m_triangulation[i].a, &m_triangulation[i].b, &m_triangulation[i].d, &m_triangulation[j])) {
			
				m_voronoiEdges.emplace_back(m_triangulation[i].circumcenter, m_triangulation[j].circumcenter);
				m_voronoiEdges[m_voronoiEdges.size() - 1].triangle = Triangle(m_triangulation[i].a, m_triangulation[i].b, m_triangulation[i].d);
			
			}
			
			if (this->IsFace(&m_triangulation[i].a, &m_triangulation[i].c, &m_triangulation[i].d, &m_triangulation[j])) {
			
				m_voronoiEdges.emplace_back(m_triangulation[i].circumcenter, m_triangulation[j].circumcenter);
				m_voronoiEdges[m_voronoiEdges.size() - 1].triangle = Triangle(m_triangulation[i].a, m_triangulation[i].c, m_triangulation[i].d);
			}
			
			if (this->IsFace(&m_triangulation[i].b, &m_triangulation[i].c, &m_triangulation[i].d, &m_triangulation[j])) {
			
				m_voronoiEdges.emplace_back(m_triangulation[i].circumcenter, m_triangulation[j].circumcenter);
				m_voronoiEdges[m_voronoiEdges.size() - 1].triangle = Triangle(m_triangulation[i].b, m_triangulation[i].c, m_triangulation[i].d);
			}
		}

	}

	wchar_t* buffer = new wchar_t[500];

	FLOAT scaleConstant = 1.0f;
	Point centeredPointA = Point();
	Point centeredPointB = Point();
	Point centeredPointC = Point();

	BOOL isEdge = FALSE;
	FLOAT colorR = 0;
	FLOAT colorG = 0;
	FLOAT colorB = 0;
	//Find voronoi polygonal face - voronoi edges
	for (UINT i = 0; i < m_delaunayEdges.size(); ++i) {

		colorR = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
		colorG = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
		colorB = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
		m_voronoiFace.clear();
		//Find face for the i-th delaunay edge
		{
		
			/*for (UINT j = 0; j < m_triangulation.size(); ++j) {

				isEdge = FALSE;
				Triangle delaunayEdgeTriangleA = {};
				Triangle delaunayEdgeTriangleB = {};
				if (m_delaunayEdges[i] == Edge(m_triangulation[j].a, m_triangulation[j].b)) {

					isEdge = TRUE;
					delaunayEdgeTriangleA.a = m_delaunayEdges[i].a;
					delaunayEdgeTriangleA.b = m_delaunayEdges[i].b;
					delaunayEdgeTriangleA.c = m_triangulation[j].c;

					delaunayEdgeTriangleA.a = m_delaunayEdges[i].a;
					delaunayEdgeTriangleA.b = m_delaunayEdges[i].b;
					delaunayEdgeTriangleA.c = m_triangulation[j].d;

				}
				if (m_delaunayEdges[i] == Edge(m_triangulation[j].a, m_triangulation[j].c))
				{

					isEdge = TRUE;
					delaunayEdgeTriangleA.a = m_delaunayEdges[i].a;
					delaunayEdgeTriangleA.b = m_delaunayEdges[i].b;
					delaunayEdgeTriangleA.c = m_triangulation[j].b;

					delaunayEdgeTriangleA.a = m_delaunayEdges[i].a;
					delaunayEdgeTriangleA.b = m_delaunayEdges[i].b;
					delaunayEdgeTriangleA.c = m_triangulation[j].d;

				}
				if (m_delaunayEdges[i] == Edge(m_triangulation[j].a, m_triangulation[j].d))
				{

					isEdge = TRUE;
					delaunayEdgeTriangleA.a = m_delaunayEdges[i].a;
					delaunayEdgeTriangleA.b = m_delaunayEdges[i].b;
					delaunayEdgeTriangleA.c = m_triangulation[j].b;

					delaunayEdgeTriangleA.a = m_delaunayEdges[i].a;
					delaunayEdgeTriangleA.b = m_delaunayEdges[i].b;
					delaunayEdgeTriangleA.c = m_triangulation[j].c;

				}
				if (m_delaunayEdges[i] == Edge(m_triangulation[j].b, m_triangulation[j].c))
				{

					isEdge = TRUE;
					delaunayEdgeTriangleA.a = m_delaunayEdges[i].a;
					delaunayEdgeTriangleA.b = m_delaunayEdges[i].b;
					delaunayEdgeTriangleA.c = m_triangulation[j].a;

					delaunayEdgeTriangleA.a = m_delaunayEdges[i].a;
					delaunayEdgeTriangleA.b = m_delaunayEdges[i].b;
					delaunayEdgeTriangleA.c = m_triangulation[j].d;

				}
				if (m_delaunayEdges[i] == Edge(m_triangulation[j].b, m_triangulation[j].d))
				{

					isEdge = TRUE;
					delaunayEdgeTriangleA.a = m_delaunayEdges[i].a;
					delaunayEdgeTriangleA.b = m_delaunayEdges[i].b;
					delaunayEdgeTriangleA.c = m_triangulation[j].a;

					delaunayEdgeTriangleA.a = m_delaunayEdges[i].a;
					delaunayEdgeTriangleA.b = m_delaunayEdges[i].b;
					delaunayEdgeTriangleA.c = m_triangulation[j].c;

				}
				if (m_delaunayEdges[i] == Edge(m_triangulation[j].c, m_triangulation[j].d))
				{

					isEdge = TRUE;
					delaunayEdgeTriangleA.a = m_delaunayEdges[i].a;
					delaunayEdgeTriangleA.b = m_delaunayEdges[i].b;
					delaunayEdgeTriangleA.c = m_triangulation[j].a;

					delaunayEdgeTriangleA.a = m_delaunayEdges[i].a;
					delaunayEdgeTriangleA.b = m_delaunayEdges[i].b;
					delaunayEdgeTriangleA.c = m_triangulation[j].b;

				}

				if (isEdge) {

					::swprintf_s(buffer, 500, L"Circumcenter: X: %f, Y: %f, Z: %f.\n", m_triangulation[j].circumcenter.x, m_triangulation[j].circumcenter.y, m_triangulation[j].circumcenter.z);
					::OutputDebugString(buffer);

				}

				for (UINT k = 0; k < m_voronoiEdges.size(); ++k) {

					if (delaunayEdgeTriangleA == m_voronoiEdges[k].triangle) {

						m_voronoiFace.emplace_back(m_voronoiEdges[k].a, m_voronoiEdges[k].b);

					}

					if (delaunayEdgeTriangleB == m_voronoiEdges[k].triangle) {

						m_voronoiFace.emplace_back(m_voronoiEdges[k].a, m_voronoiEdges[k].b);

					}

				}

			}*/

		}
		
		//Find face for the i-th delaunay edge - other method
		for (UINT j = 0; j < m_voronoiEdges.size(); ++j) {

			isEdge = FALSE;
			if (m_delaunayEdges[i] == Edge(m_voronoiEdges[j].triangle.a, m_voronoiEdges[j].triangle.b)) isEdge = TRUE;
			if (m_delaunayEdges[i] == Edge(m_voronoiEdges[j].triangle.a, m_voronoiEdges[j].triangle.c)) isEdge = TRUE;
			if (m_delaunayEdges[i] == Edge(m_voronoiEdges[j].triangle.b, m_voronoiEdges[j].triangle.c)) isEdge = TRUE;
			
			if (isEdge) {

				m_voronoiFace.emplace_back(m_voronoiEdges[j].a, m_voronoiEdges[j].b);

			}

		}

		for (UINT j = 0; j < m_voronoiFace.size(); ++j) {

			m_voronoiColoredEdges.emplace_back(m_voronoiFace[j].a, m_voronoiFace[j].b, Point(colorR, colorG, colorB));

		}

		for (UINT j = 0; j < m_voronoiFace.size(); ++j) {

			m_voronoiFaces.push_back(m_voronoiFace[j]);

		}
		m_voronoiFacesCount.emplace_back(static_cast<UINT>(m_voronoiFace.size()));

		if (m_voronoiFace.size() > 2) {

			m_voronoiFaceOrdered.clear();
			m_voronoiFaceOrdered.push_back(m_voronoiFace[0]);
			m_voronoiFace.erase(m_voronoiFace.begin());

			while (m_voronoiFace.size() > 0) {

				for (UINT j = 0; j < m_voronoiFace.size(); ++j) {

					if (m_voronoiFace[j].a == m_voronoiFaceOrdered[m_voronoiFaceOrdered.size() - 1].b) {

						m_voronoiFaceOrdered.emplace_back(m_voronoiFace[j]);
						m_voronoiFace.erase(m_voronoiFace.begin() + j);
						break;

					}

					if (m_voronoiFace[j].b == m_voronoiFaceOrdered[m_voronoiFaceOrdered.size() - 1].b) {

						m_voronoiFaceOrdered.emplace_back(m_voronoiFace[j].b, m_voronoiFace[j].a);
						m_voronoiFace.erase(m_voronoiFace.begin() + j);
						break;

					}

				}

			}

			for (UINT j = 2; j < m_voronoiFaceOrdered.size(); ++j) {

				//m_voronoiColoredTriangles.emplace_back(m_voronoiFaceOrdered[0].a, m_voronoiFaceOrdered[j - 1].a, m_voronoiFaceOrdered[j].a, Point(colorR, colorG, colorB));

				centeredPointA = m_voronoiFaceOrdered[0].a;
				centeredPointB = m_voronoiFaceOrdered[j - 1].a;
				centeredPointC = m_voronoiFaceOrdered[j].a;

				centeredPointA -= m_delaunayEdges[i].a;
				centeredPointB -= m_delaunayEdges[i].a;
				centeredPointC -= m_delaunayEdges[i].a;

				centeredPointA *= scaleConstant;
				centeredPointB *= scaleConstant;
				centeredPointC *= scaleConstant;

				centeredPointA += m_delaunayEdges[i].a;
				centeredPointB += m_delaunayEdges[i].a;
				centeredPointC += m_delaunayEdges[i].a;

				//m_voronoiColoredTriangles.emplace_back(centeredPointA, centeredPointB, centeredPointC, m_delaunayEdges[i].a);
				m_voronoiColoredTriangles.emplace_back(centeredPointA, centeredPointB, centeredPointC, Point(colorR, colorG, colorB));

				centeredPointA = m_voronoiFaceOrdered[0].a;
				centeredPointB = m_voronoiFaceOrdered[j - 1].a;
				centeredPointC = m_voronoiFaceOrdered[j].a;

				centeredPointA -= m_delaunayEdges[i].b;
				centeredPointB -= m_delaunayEdges[i].b;
				centeredPointC -= m_delaunayEdges[i].b;

				centeredPointA *= scaleConstant;
				centeredPointB *= scaleConstant;
				centeredPointC *= scaleConstant;

				centeredPointA += m_delaunayEdges[i].b;
				centeredPointB += m_delaunayEdges[i].b;
				centeredPointC += m_delaunayEdges[i].b;

				//m_voronoiColoredTriangles.emplace_back(centeredPointA, centeredPointB, centeredPointC, m_delaunayEdges[i].b);
				m_voronoiColoredTriangles.emplace_back(centeredPointA, centeredPointB, centeredPointC, Point(colorR, colorG, colorB));

			}

			m_voronoiCells[m_delaunayEdgesIndex[i].a].emplace_back();
			m_voronoiCells[m_delaunayEdgesIndex[i].b].emplace_back();
			for (UINT j = 0; j < m_voronoiFaceOrdered.size(); ++j) {

				m_voronoiCells[m_delaunayEdgesIndex[i].a][m_voronoiCells[m_delaunayEdgesIndex[i].a].size() - 1].emplace_back(m_voronoiFaceOrdered[j].a, m_voronoiFaceOrdered[j].b);
				m_voronoiCells[m_delaunayEdgesIndex[i].b][m_voronoiCells[m_delaunayEdgesIndex[i].b].size() - 1].emplace_back(m_voronoiFaceOrdered[j].a, m_voronoiFaceOrdered[j].b);

			}

			m_separateVoronoiFaces.emplace_back();
			for (UINT j = 0; j < m_voronoiFaceOrdered.size(); ++j) {

				m_separateVoronoiFaces[m_separateVoronoiFaces.size() - 1].emplace_back(m_voronoiFaceOrdered[j].a, m_voronoiFaceOrdered[j].b);

			}
			m_pointedVoronoiCells[m_delaunayEdgesIndex[i].a].emplace_back(i);
			m_pointedVoronoiCells[m_delaunayEdgesIndex[i].b].emplace_back(i);

			m_separateVoronoiFaceColors.emplace_back(colorR, colorG, colorB);

		}

	}

	Point planePointA = {};
	Point planePointB = {};
	Point planePointC = {};

	DirectX::XMVECTOR vectorAB = {};
	DirectX::XMVECTOR vectorBC = {};
	DirectX::XMVECTOR planeNormal = {};

	FLOAT planeXCoef = 0.0f;
	FLOAT planeYCoef = 0.0f;
	FLOAT planeZCoef = 0.0f;
	FLOAT planeKCoef = 0.0f;

	Point lineA = {};
	Point lineB = {};
	Point intersection = {};
	BOOL intersectsPlane = FALSE;

	FLOAT crossSectionCetroidX = 0.0f;
	FLOAT crossSectionCetroidY = 0.0f;
	FLOAT crossSectionCetroidZ = 0.0f;

	FLOAT voronoiFaceCentroidX = 0.0f;
	FLOAT voronoiFaceCentroidY = 0.0f;
	FLOAT voronoiFaceCentroidZ = 0.0f;

	DirectX::XMVECTOR vectorCentroidA = {};
	DirectX::XMVECTOR vectorCentroidB = {};
	DirectX::XMVECTOR vectorCentroidX = {};
	DirectX::XMVECTOR vectorCentroidDotProduct = {};
	DirectX::XMVECTOR vectorCentroidBNormal = {};
	DirectX::XMVECTOR vectorCentroidXNormal = {};
	DirectX::XMVECTOR vectorCentroidNormalDotProduct = {};
	DirectX::XMVECTOR vectorCentroidANormalized = {};
	DirectX::XMVECTOR vectorCentroidXNormalized = {};

	FLOAT magnitudeA = 0.0f;
	FLOAT magnitudeB = 0.0f;
	FLOAT magnitudeX = 0.0f;
	FLOAT crossSectionVectorAngle = 0.0f;
	FLOAT angleSign = 0.0f;

	FLOAT angleMin = 1000.0f;
	UINT angleMinIndex = 0;

	BOOL toClip = FALSE;

	DirectX::XMVECTOR vectorEdgeBA = {};
	DirectX::XMVECTOR vectorEdgeCA = {};
	DirectX::XMVECTOR vectorEdgeBACADot = {};

	FLOAT edgeDotProduct = 0.0f;
	FLOAT edgeBADistanceSq = 0.0f;

	BOOL singleEdge = FALSE;
	BOOL doubleEdge = FALSE;
	BOOL intersectsUnitCube = FALSE;

	Edge intersectingEdgeA = {};
	Edge intersectingEdgeB = {};

	UINT centroidIndex = 0;
	BOOL centroidFound = FALSE;

	Point projectedCentroid = {};
	BOOL centroidProjectionFlipped = FALSE;
	Point centroidPoint = {};
	INT voronoiFaceUnitCubeIntersectionNum = 0;

	//Pre-calculate the plane equations of the voronoi faces
	for (UINT i = 0; i < m_separateVoronoiFaces.size(); ++i) {

		//Three point on the plane
		planePointA = m_separateVoronoiFaces[i][0].a;
		planePointB = m_separateVoronoiFaces[i][1].a;
		planePointC = m_separateVoronoiFaces[i][2].a;

		//Vectors A - B; B - C
		vectorAB = DirectX::XMVectorSet(planePointA.x - planePointB.x, planePointA.y - planePointB.y, planePointA.z - planePointB.z, 1.0f);
		vectorBC = DirectX::XMVectorSet(planePointB.x - planePointC.x, planePointB.y - planePointC.y, planePointB.z - planePointC.z, 1.0f);

		//Cross product of AB x BC
		planeNormal = DirectX::XMVector3Cross(vectorAB, vectorBC);

		//Equation of plane ABC: planeNormal.x * x + planeNormal.y * y + planeNormal.z * z + k = 0
		//Solve for A to get k: k = -(planeNormal.x * x + planeNormal.y * y + planeNormal.z * z)
		planeXCoef = DirectX::XMVectorGetX(planeNormal);
		planeYCoef = DirectX::XMVectorGetY(planeNormal);
		planeZCoef = DirectX::XMVectorGetZ(planeNormal);
		planeKCoef = -(planeXCoef * planePointA.x + planeYCoef * planePointA.y + planeZCoef * planePointA.z);

		m_separateVoronoiFacePlanes.emplace_back(planeXCoef, planeYCoef, planeZCoef, planeKCoef);

	}

	//How many faces pre-clipping
	preClipFaceCount = static_cast<UINT>(m_separateVoronoiFaces.size());

	//Clip voronoi faces to the unit cube
	if (TRUE) {

		for (UINT i = 0; i < m_separateVoronoiFaces.size(); ++i) {

			m_separateVoronoiFacesCulledOrdered.emplace_back();
			m_cubeCrossSection.clear();

			colorR = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorG = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorB = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;

			//Check for edges with vertices over 1.0f or under 0.0f
			toClip = FALSE;
			singleEdge = FALSE;
			doubleEdge = FALSE;
			voronoiFaceUnitCubeIntersectionNum = 0;
			for (UINT j = 0; j < m_separateVoronoiFaces[i].size(); ++j) {

				intersectsUnitCube = FALSE;
				if (m_separateVoronoiFaces[i][j].a.x >= 1.0f || m_separateVoronoiFaces[i][j].a.y >= 1.0f || m_separateVoronoiFaces[i][j].a.z >= 1.0f ||
					m_separateVoronoiFaces[i][j].a.x <= 0.0f || m_separateVoronoiFaces[i][j].a.y <= 0.0f || m_separateVoronoiFaces[i][j].a.z <= 0.0f ||
					m_separateVoronoiFaces[i][j].b.x >= 1.0f || m_separateVoronoiFaces[i][j].b.y >= 1.0f || m_separateVoronoiFaces[i][j].b.z >= 1.0f ||
					m_separateVoronoiFaces[i][j].b.x <= 0.0f || m_separateVoronoiFaces[i][j].b.y <= 0.0f || m_separateVoronoiFaces[i][j].b.z <= 0.0f)
				{

					//Check if line intersects the planes of the unit cube

					vectorEdgeBA = DirectX::XMVectorSet(
						m_separateVoronoiFaces[i][j].b.x - m_separateVoronoiFaces[i][j].a.x,
						m_separateVoronoiFaces[i][j].b.y - m_separateVoronoiFaces[i][j].a.y,
						m_separateVoronoiFaces[i][j].b.z - m_separateVoronoiFaces[i][j].a.z,
						1.0f);
					edgeBADistanceSq =
						(m_separateVoronoiFaces[i][j].b.x - m_separateVoronoiFaces[i][j].a.x) * (m_separateVoronoiFaces[i][j].b.x - m_separateVoronoiFaces[i][j].a.x) +
						(m_separateVoronoiFaces[i][j].b.y - m_separateVoronoiFaces[i][j].a.y) * (m_separateVoronoiFaces[i][j].b.y - m_separateVoronoiFaces[i][j].a.y) +
						(m_separateVoronoiFaces[i][j].b.z - m_separateVoronoiFaces[i][j].a.z) * (m_separateVoronoiFaces[i][j].b.z - m_separateVoronoiFaces[i][j].a.z);

					//Unit cube plane 1.
					lineA = m_separateVoronoiFaces[i][j].a; lineB = m_separateVoronoiFaces[i][j].b;
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, 0.0f, 0.0f, 1.0f, 0.0f, &intersection);
					if (intersectsPlane == TRUE &&
						intersection.x >= 0.0f && intersection.x <= 1.0f &&
						intersection.y >= 0.0f && intersection.y <= 1.0f)
					{

						vectorEdgeCA = DirectX::XMVectorSet(
							m_separateVoronoiFaces[i][j].b.x - intersection.x,
							m_separateVoronoiFaces[i][j].b.y - intersection.y,
							m_separateVoronoiFaces[i][j].b.z - intersection.z,
							1.0f);

						vectorEdgeBACADot = DirectX::XMVector3Dot(vectorEdgeBA, vectorEdgeCA);
						edgeDotProduct = DirectX::XMVectorGetX(vectorEdgeBACADot);

						if (edgeDotProduct > 0.0f && edgeDotProduct < edgeBADistanceSq) {

							swprintf_s(buffer, 500, L"Edge A: %f, %f, %f; B: %f, %f, %f; intersects plane xy k = 0 in point: %f, %f, %f.\n",
								m_separateVoronoiFaces[i][j].a.x, m_separateVoronoiFaces[i][j].a.y, m_separateVoronoiFaces[i][j].a.z,
								m_separateVoronoiFaces[i][j].b.x, m_separateVoronoiFaces[i][j].b.y, m_separateVoronoiFaces[i][j].b.z,
								intersection.x, intersection.y, intersection.z);
							OutputDebugString(buffer);
							m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);
							intersectsUnitCube = TRUE;
							++voronoiFaceUnitCubeIntersectionNum;
							toClip = TRUE;

						}

					}


					//Unit cube plane 2.
					lineA = m_separateVoronoiFaces[i][j].a; lineB = m_separateVoronoiFaces[i][j].b;
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, 0.0f, 0.0f, 1.0f, -1.0f, &intersection);
					if (intersectsPlane == TRUE &&
						intersection.x >= 0.0f && intersection.x <= 1.0f &&
						intersection.y >= 0.0f && intersection.y <= 1.0f)
					{

						vectorEdgeCA = DirectX::XMVectorSet(
							m_separateVoronoiFaces[i][j].b.x - intersection.x,
							m_separateVoronoiFaces[i][j].b.y - intersection.y,
							m_separateVoronoiFaces[i][j].b.z - intersection.z,
							1.0f);

						vectorEdgeBACADot = DirectX::XMVector3Dot(vectorEdgeBA, vectorEdgeCA);
						edgeDotProduct = DirectX::XMVectorGetX(vectorEdgeBACADot);

						if (edgeDotProduct > 0.0f && edgeDotProduct < edgeBADistanceSq) {

							swprintf_s(buffer, 500, L"Edge A: %f, %f, %f; B: %f, %f, %f; intersects plane xy k = -1 in point: %f, %f, %f.\n",
								m_separateVoronoiFaces[i][j].a.x, m_separateVoronoiFaces[i][j].a.y, m_separateVoronoiFaces[i][j].a.z,
								m_separateVoronoiFaces[i][j].b.x, m_separateVoronoiFaces[i][j].b.y, m_separateVoronoiFaces[i][j].b.z,
								intersection.x, intersection.y, intersection.z);
							OutputDebugString(buffer);
							m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);
							intersectsUnitCube = TRUE;
							++voronoiFaceUnitCubeIntersectionNum;
							toClip = TRUE;

						}

					}

					//Unit cube plane 3.
					lineA = m_separateVoronoiFaces[i][j].a; lineB = m_separateVoronoiFaces[i][j].b;
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, 1.0f, 0.0f, 0.0f, 0.0f, &intersection);
					if (intersectsPlane == TRUE &&
						intersection.y >= 0.0f && intersection.y <= 1.0f &&
						intersection.z >= 0.0f && intersection.z <= 1.0f)
					{

						vectorEdgeCA = DirectX::XMVectorSet(
							m_separateVoronoiFaces[i][j].b.x - intersection.x,
							m_separateVoronoiFaces[i][j].b.y - intersection.y,
							m_separateVoronoiFaces[i][j].b.z - intersection.z,
							1.0f);

						vectorEdgeBACADot = DirectX::XMVector3Dot(vectorEdgeBA, vectorEdgeCA);
						edgeDotProduct = DirectX::XMVectorGetX(vectorEdgeBACADot);

						if (edgeDotProduct > 0.0f && edgeDotProduct < edgeBADistanceSq) {

							swprintf_s(buffer, 500, L"Edge A: %f, %f, %f; B: %f, %f, %f; intersects plane yz k = 0 in point: %f, %f, %f.\n",
								m_separateVoronoiFaces[i][j].a.x, m_separateVoronoiFaces[i][j].a.y, m_separateVoronoiFaces[i][j].a.z,
								m_separateVoronoiFaces[i][j].b.x, m_separateVoronoiFaces[i][j].b.y, m_separateVoronoiFaces[i][j].b.z,
								intersection.x, intersection.y, intersection.z);
							OutputDebugString(buffer);
							m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);
							intersectsUnitCube = TRUE;
							++voronoiFaceUnitCubeIntersectionNum;
							toClip = TRUE;

						}

					}

					//Unit cube plane 4.
					lineA = m_separateVoronoiFaces[i][j].a; lineB = m_separateVoronoiFaces[i][j].b;
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, 1.0f, 0.0f, 0.0f, -1.0f, &intersection);
					if (intersectsPlane == TRUE &&
						intersection.y >= 0.0f && intersection.y <= 1.0f &&
						intersection.z >= 0.0f && intersection.z <= 1.0f)
					{

						vectorEdgeCA = DirectX::XMVectorSet(
							m_separateVoronoiFaces[i][j].b.x - intersection.x,
							m_separateVoronoiFaces[i][j].b.y - intersection.y,
							m_separateVoronoiFaces[i][j].b.z - intersection.z,
							1.0f);

						vectorEdgeBACADot = DirectX::XMVector3Dot(vectorEdgeBA, vectorEdgeCA);
						edgeDotProduct = DirectX::XMVectorGetX(vectorEdgeBACADot);

						if (edgeDotProduct > 0.0f && edgeDotProduct < edgeBADistanceSq) {

							swprintf_s(buffer, 500, L"Edge A: %f, %f, %f; B: %f, %f, %f; intersects plane yz k = -1 in point: %f, %f, %f.\n",
								m_separateVoronoiFaces[i][j].a.x, m_separateVoronoiFaces[i][j].a.y, m_separateVoronoiFaces[i][j].a.z,
								m_separateVoronoiFaces[i][j].b.x, m_separateVoronoiFaces[i][j].b.y, m_separateVoronoiFaces[i][j].b.z,
								intersection.x, intersection.y, intersection.z);
							OutputDebugString(buffer);
							m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);
							intersectsUnitCube = TRUE;
							++voronoiFaceUnitCubeIntersectionNum;
							toClip = TRUE;

						}

					}

					//Unit cube plane 5.
					lineA = m_separateVoronoiFaces[i][j].a; lineB = m_separateVoronoiFaces[i][j].b;
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, 0.0f, 1.0f, 0.0f, 0.0f, &intersection);
					if (intersectsPlane == TRUE &&
						intersection.x >= 0.0f && intersection.x <= 1.0f &&
						intersection.z >= 0.0f && intersection.z <= 1.0f)
					{

						vectorEdgeCA = DirectX::XMVectorSet(
							m_separateVoronoiFaces[i][j].b.x - intersection.x,
							m_separateVoronoiFaces[i][j].b.y - intersection.y,
							m_separateVoronoiFaces[i][j].b.z - intersection.z,
							1.0f);

						vectorEdgeBACADot = DirectX::XMVector3Dot(vectorEdgeBA, vectorEdgeCA);
						edgeDotProduct = DirectX::XMVectorGetX(vectorEdgeBACADot);

						if (edgeDotProduct > 0.0f && edgeDotProduct < edgeBADistanceSq) {

							swprintf_s(buffer, 500, L"Edge A: %f, %f, %f; B: %f, %f, %f; intersects plane xz k = 0 in point: %f, %f, %f.\n",
								m_separateVoronoiFaces[i][j].a.x, m_separateVoronoiFaces[i][j].a.y, m_separateVoronoiFaces[i][j].a.z,
								m_separateVoronoiFaces[i][j].b.x, m_separateVoronoiFaces[i][j].b.y, m_separateVoronoiFaces[i][j].b.z,
								intersection.x, intersection.y, intersection.z);
							OutputDebugString(buffer);
							m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);
							intersectsUnitCube = TRUE;
							++voronoiFaceUnitCubeIntersectionNum;
							toClip = TRUE;

						}

					}

					//Unit cube plane 6.
					lineA = m_separateVoronoiFaces[i][j].a; lineB = m_separateVoronoiFaces[i][j].b;
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, 0.0f, 1.0f, 0.0f, -1.0f, &intersection);
					if (intersectsPlane == TRUE &&
						intersection.x >= 0.0f && intersection.x <= 1.0f &&
						intersection.z >= 0.0f && intersection.z <= 1.0f)
					{

						vectorEdgeCA = DirectX::XMVectorSet(
							m_separateVoronoiFaces[i][j].b.x - intersection.x,
							m_separateVoronoiFaces[i][j].b.y - intersection.y,
							m_separateVoronoiFaces[i][j].b.z - intersection.z,
							1.0f);

						vectorEdgeBACADot = DirectX::XMVector3Dot(vectorEdgeBA, vectorEdgeCA);
						edgeDotProduct = DirectX::XMVectorGetX(vectorEdgeBACADot);

						if (edgeDotProduct > 0.0f && edgeDotProduct < edgeBADistanceSq) {

							swprintf_s(buffer, 500, L"Edge A: %f, %f, %f; B: %f, %f, %f; intersects plane xz k = -1 in point: %f, %f, %f.\n",
								m_separateVoronoiFaces[i][j].a.x, m_separateVoronoiFaces[i][j].a.y, m_separateVoronoiFaces[i][j].a.z,
								m_separateVoronoiFaces[i][j].b.x, m_separateVoronoiFaces[i][j].b.y, m_separateVoronoiFaces[i][j].b.z,
								intersection.x, intersection.y, intersection.z);
							OutputDebugString(buffer);
							m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);
							intersectsUnitCube = TRUE;
							++voronoiFaceUnitCubeIntersectionNum;
							toClip = TRUE;

						}

					}

				}

				if ((intersectsUnitCube == TRUE) && (singleEdge == FALSE)) {

					singleEdge = TRUE;
					intersectingEdgeA = m_separateVoronoiFaces[i][j];

				}
				else if ((intersectsUnitCube == TRUE) && (singleEdge == TRUE)) {

					doubleEdge = TRUE;
					singleEdge = FALSE;
					intersectingEdgeB = m_separateVoronoiFaces[i][j];

				}

			}

			//Resolve rounding errors
			for (UINT j = 0; j < m_cubeCrossSection.size(); ++j) {

				if (m_cubeCrossSection[j].x >= 0.999999000f) m_cubeCrossSection[j].x = 1.0f;
				if (m_cubeCrossSection[j].y >= 0.999999000f) m_cubeCrossSection[j].y = 1.0f;
				if (m_cubeCrossSection[j].z >= 0.999999000f) m_cubeCrossSection[j].z = 1.0f;

				if (m_cubeCrossSection[j].x <= 0.000000100f) m_cubeCrossSection[j].x = 0.0f;
				if (m_cubeCrossSection[j].y <= 0.000000100f) m_cubeCrossSection[j].y = 0.0f;
				if (m_cubeCrossSection[j].z <= 0.000000100f) m_cubeCrossSection[j].z = 0.0f;

			}

			//Get voronoi face plane equation

			//Three point on the plane
			planePointA = m_separateVoronoiFaces[i][0].a;
			planePointB = m_separateVoronoiFaces[i][1].a;
			planePointC = m_separateVoronoiFaces[i][2].a;

			//Vectors A - B; B - C
			vectorAB = DirectX::XMVectorSet(planePointA.x - planePointB.x, planePointA.y - planePointB.y, planePointA.z - planePointB.z, 1.0f);
			vectorBC = DirectX::XMVectorSet(planePointB.x - planePointC.x, planePointB.y - planePointC.y, planePointB.z - planePointC.z, 1.0f);

			//Cross product of AB x BC
			planeNormal = DirectX::XMVector3Cross(vectorAB, vectorBC);

			//Equation of plane ABC: planeNormal.x * x + planeNormal.y * y + planeNormal.z * z + k = 0
			//Solve for A to get k: k = -(planeNormal.x * x + planeNormal.y * y + planeNormal.z * z)
			planeXCoef = DirectX::XMVectorGetX(planeNormal);
			planeYCoef = DirectX::XMVectorGetY(planeNormal);
			planeZCoef = DirectX::XMVectorGetZ(planeNormal);
			planeKCoef = -(planeXCoef * planePointA.x + planeYCoef * planePointA.y + planeZCoef * planePointA.z);

			m_cubeCrossSectionCentroidAngle.clear();
			m_cubeCrossSectionOrdered.clear();

			if ((toClip == TRUE) || (toClip == FALSE)) {

				//Check if plane intersects the 12 edges of the (0, 1) (0, 1) (0, 1) cube
				{

					//Edge 1.
					lineA = Point(0.0f, 0.0f, 0.0f); lineB = Point(0.0f, 1.0f, 0.0f);
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, planeXCoef, planeYCoef, planeZCoef, planeKCoef, &intersection);
					if (intersectsPlane == TRUE && intersection.y >= 0.0f && intersection.y <= 1.0f) m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);

					//Edge 2.
					lineA = Point(0.0f, 0.0f, 0.0f); lineB = Point(1.0f, 0.0f, 0.0f);
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, planeXCoef, planeYCoef, planeZCoef, planeKCoef, &intersection);
					if (intersectsPlane == TRUE && intersection.x >= 0.0f && intersection.x <= 1.0f) m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);

					//Edge 3.
					lineA = Point(1.0f, 1.0f, 0.0f); lineB = Point(0.0f, 1.0f, 0.0f);
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, planeXCoef, planeYCoef, planeZCoef, planeKCoef, &intersection);
					if (intersectsPlane == TRUE && intersection.x >= 0.0f && intersection.x <= 1.0f) m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);

					//Edge 4.
					lineA = Point(1.0f, 1.0f, 0.0f); lineB = Point(1.0f, 0.0f, 0.0f);
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, planeXCoef, planeYCoef, planeZCoef, planeKCoef, &intersection);
					if (intersectsPlane == TRUE && intersection.y >= 0.0f && intersection.y <= 1.0f) m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);

					//Edge 5.
					lineA = Point(0.0f, 0.0f, 1.0f); lineB = Point(0.0f, 1.0f, 1.0f);
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, planeXCoef, planeYCoef, planeZCoef, planeKCoef, &intersection);
					if (intersectsPlane == TRUE && intersection.y >= 0.0f && intersection.y <= 1.0f) m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);

					//Edge 6.
					lineA = Point(0.0f, 0.0f, 1.0f); lineB = Point(1.0f, 0.0f, 1.0f);
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, planeXCoef, planeYCoef, planeZCoef, planeKCoef, &intersection);
					if (intersectsPlane == TRUE && intersection.x >= 0.0f && intersection.x <= 1.0f) m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);

					//Edge 7.
					lineA = Point(1.0f, 1.0f, 1.0f); lineB = Point(0.0f, 1.0f, 1.0f);
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, planeXCoef, planeYCoef, planeZCoef, planeKCoef, &intersection);
					if (intersectsPlane == TRUE && intersection.x >= 0.0f && intersection.x <= 1.0f) m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);

					//Edge 8.
					lineA = Point(1.0f, 1.0f, 1.0f); lineB = Point(1.0f, 0.0f, 1.0f);
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, planeXCoef, planeYCoef, planeZCoef, planeKCoef, &intersection);
					if (intersectsPlane == TRUE && intersection.y >= 0.0f && intersection.y <= 1.0f) m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);

					//Edge 9.
					lineA = Point(0.0f, 0.0f, 0.0f); lineB = Point(0.0f, 0.0f, 1.0f);
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, planeXCoef, planeYCoef, planeZCoef, planeKCoef, &intersection);
					if (intersectsPlane == TRUE && intersection.z >= 0.0f && intersection.z <= 1.0f) m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);

					//Edge 10.
					lineA = Point(0.0f, 1.0f, 0.0f); lineB = Point(0.0f, 1.0f, 1.0f);
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, planeXCoef, planeYCoef, planeZCoef, planeKCoef, &intersection);
					if (intersectsPlane == TRUE && intersection.z >= 0.0f && intersection.z <= 1.0f) m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);

					//Edge 11.
					lineA = Point(1.0f, 1.0f, 0.0f); lineB = Point(1.0f, 1.0f, 1.0f);
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, planeXCoef, planeYCoef, planeZCoef, planeKCoef, &intersection);
					if (intersectsPlane == TRUE && intersection.z >= 0.0f && intersection.z <= 1.0f) m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);

					//Edge 12.
					lineA = Point(1.0f, 0.0f, 0.0f); lineB = Point(1.0f, 0.0f, 1.0f);
					intersectsPlane = this->LinePlaneIntersection(&lineA, &lineB, planeXCoef, planeYCoef, planeZCoef, planeKCoef, &intersection);
					if (intersectsPlane == TRUE && intersection.z >= 0.0f && intersection.z <= 1.0f) m_cubeCrossSection.emplace_back(intersection.x, intersection.y, intersection.z);

				}

				//Project centroid onto plane
				{

					////Find centroid
					//{

					//	centroidFound = FALSE;
					//	UINT j = 0;
					//	while (centroidFound == FALSE) {

					//		for (UINT k = 0; k < m_pointedVoronoiCells[j].size(); ++k) {

					//			if (m_pointedVoronoiCells[j][k] == i) {

					//				centroidFound = TRUE;
					//				centroidIndex = j;

					//			}

					//		}
					//		++j;

					//	}

					//}

					////Projection
					//{
					//	FLOAT t =
					//		planeXCoef * planePointA.x - planeXCoef * m_centroids[centroidIndex].x +
					//		planeYCoef * planePointA.y - planeYCoef * m_centroids[centroidIndex].y +
					//		planeZCoef * planePointA.z - planeZCoef * m_centroids[centroidIndex].z;
					//	t = t / (planeXCoef * planeXCoef + planeYCoef * planeYCoef + planeZCoef * planeZCoef);
					//	projectedCentroid = Point(m_centroids[centroidIndex].x + planeXCoef * t, m_centroids[centroidIndex].y + planeYCoef * t, m_centroids[centroidIndex].z + planeZCoef * t);

					//}
				
				}

				//Centroid Projeciton method
				{

					////Check if the edge of the centroid and its projection intersects another voronoi face associated with the centroid
					//centroidProjectionFlipped = FALSE;
					//vectorEdgeBA = DirectX::XMVectorSet(
					//	projectedCentroid.x - m_centroids[centroidIndex].x,
					//	projectedCentroid.y - m_centroids[centroidIndex].y,
					//	projectedCentroid.z - m_centroids[centroidIndex].z,
					//	1.0f);
					//edgeBADistanceSq =
					//	(projectedCentroid.x - m_centroids[centroidIndex].x) * (projectedCentroid.x - m_centroids[centroidIndex].x) +
					//	(projectedCentroid.y - m_centroids[centroidIndex].y) * (projectedCentroid.y - m_centroids[centroidIndex].y) +
					//	(projectedCentroid.z - m_centroids[centroidIndex].z) * (projectedCentroid.z - m_centroids[centroidIndex].z);
					//for (UINT j = 0; j < m_pointedVoronoiCells[centroidIndex].size(); ++j) {

					//	if (m_pointedVoronoiCells[centroidIndex][j] != i) {

					//		centroidPoint = Point(m_centroids[centroidIndex].x, m_centroids[centroidIndex].y, m_centroids[centroidIndex].z);
					//		intersectsPlane = this->LinePlaneIntersection(&projectedCentroid, &centroidPoint,
					//			m_separateVoronoiFacePlanes[m_pointedVoronoiCells[centroidIndex][j]].xCoef,
					//			m_separateVoronoiFacePlanes[m_pointedVoronoiCells[centroidIndex][j]].yCoef,
					//			m_separateVoronoiFacePlanes[m_pointedVoronoiCells[centroidIndex][j]].zCoef,
					//			m_separateVoronoiFacePlanes[m_pointedVoronoiCells[centroidIndex][j]].kCoef,
					//			&intersection);

					//		if (intersectsPlane == TRUE) {

					//			vectorEdgeCA = DirectX::XMVectorSet(
					//				projectedCentroid.x - intersection.x,
					//				projectedCentroid.y - intersection.y,
					//				projectedCentroid.z - intersection.z,
					//				1.0f);

					//			vectorEdgeBACADot = DirectX::XMVector3Dot(vectorEdgeBA, vectorEdgeCA);
					//			edgeDotProduct = DirectX::XMVectorGetX(vectorEdgeBACADot);

					//			if (edgeDotProduct > 0.0f && edgeDotProduct < edgeBADistanceSq) {

					//				centroidProjectionFlipped = TRUE;

					//			}

					//		}

					//	}

					//}

					//if (singleEdge == TRUE) {

					//	//Two vectors one the edge the other from one vertex of the edge to the centroid projection
					//	vectorCentroidA = DirectX::XMVectorSet(intersectingEdgeA.b.x - intersectingEdgeA.a.x, intersectingEdgeA.b.y - intersectingEdgeA.a.y, intersectingEdgeA.b.z - intersectingEdgeA.a.z, 1.0f);
					//	vectorCentroidB = DirectX::XMVectorSet(intersectingEdgeA.b.x - projectedCentroid.x, intersectingEdgeA.b.y - projectedCentroid.y, intersectingEdgeA.b.z - projectedCentroid.z, 1.0f);

					//	vectorCentroidBNormal = DirectX::XMVector3Cross(vectorCentroidB, vectorCentroidA);

					//	for (INT j = static_cast<INT>(m_cubeCrossSection.size()) - 1; j >= 2; --j) {

					//		vectorCentroidX = DirectX::XMVectorSet(intersectingEdgeA.b.x - m_cubeCrossSection[j].x, intersectingEdgeA.b.y - m_cubeCrossSection[j].y, intersectingEdgeA.b.z - m_cubeCrossSection[j].z, 1.0f);
					//		vectorCentroidXNormal = DirectX::XMVector3Cross(vectorCentroidX, vectorCentroidA);
					//		vectorCentroidNormalDotProduct = DirectX::XMVector3Dot(vectorCentroidXNormal, vectorCentroidBNormal);
					//		if (centroidProjectionFlipped == FALSE) {

					//			if (DirectX::XMVectorGetX(vectorCentroidNormalDotProduct) < 0.0f) {

					//				m_cubeCrossSection.erase(m_cubeCrossSection.begin() + j);

					//			}

					//		}
					//		else {

					//			if (DirectX::XMVectorGetX(vectorCentroidNormalDotProduct) > 0.0f) {

					//				m_cubeCrossSection.erase(m_cubeCrossSection.begin() + j);

					//			}

					//		}


					//	}

					//}

					//if (doubleEdge == TRUE) {



					//}

				}

				voronoiFaceCentroidX = 0.0f;
				voronoiFaceCentroidY = 0.0f;
				voronoiFaceCentroidZ = 0.0f;

				for (UINT j = 0; j < m_separateVoronoiFaces[i].size(); ++j) {

					voronoiFaceCentroidX += m_separateVoronoiFaces[i][j].a.x;
					voronoiFaceCentroidY += m_separateVoronoiFaces[i][j].a.y;
					voronoiFaceCentroidZ += m_separateVoronoiFaces[i][j].a.z;

				}

				voronoiFaceCentroidX /= static_cast<FLOAT>(m_separateVoronoiFaces[i].size());
				voronoiFaceCentroidY /= static_cast<FLOAT>(m_separateVoronoiFaces[i].size());
				voronoiFaceCentroidZ /= static_cast<FLOAT>(m_separateVoronoiFaces[i].size());

				for (UINT j = 0; j < m_separateVoronoiFaces[i].size(); ++j) {

					vectorCentroidA = DirectX::XMVectorSet(
						m_separateVoronoiFaces[i][j].b.x - m_separateVoronoiFaces[i][j].a.x,
						m_separateVoronoiFaces[i][j].b.y - m_separateVoronoiFaces[i][j].a.y,
						m_separateVoronoiFaces[i][j].b.z - m_separateVoronoiFaces[i][j].a.z,
						1.0f);
					vectorCentroidB = DirectX::XMVectorSet(
						m_separateVoronoiFaces[i][j].b.x - voronoiFaceCentroidX,
						m_separateVoronoiFaces[i][j].b.y - voronoiFaceCentroidY,
						m_separateVoronoiFaces[i][j].b.z - voronoiFaceCentroidZ,
						1.0f);

					vectorCentroidBNormal = DirectX::XMVector3Cross(vectorCentroidB, vectorCentroidA);

					for (INT k = static_cast<INT>(m_cubeCrossSection.size()) - 1; k >= voronoiFaceUnitCubeIntersectionNum; --k) {

						vectorCentroidX = DirectX::XMVectorSet(
							m_separateVoronoiFaces[i][j].b.x - m_cubeCrossSection[k].x,
							m_separateVoronoiFaces[i][j].b.y - m_cubeCrossSection[k].y,
							m_separateVoronoiFaces[i][j].b.z - m_cubeCrossSection[k].z,
							1.0f);
						vectorCentroidXNormal = DirectX::XMVector3Cross(vectorCentroidX, vectorCentroidA);
						vectorCentroidNormalDotProduct = DirectX::XMVector3Dot(vectorCentroidXNormal, vectorCentroidBNormal);
						if (DirectX::XMVectorGetX(vectorCentroidNormalDotProduct) < 0.0f) {

							m_cubeCrossSection.erase(m_cubeCrossSection.begin() + k);

						}

					}
				}

			}
			/*else {

				for (UINT j = 0; j < m_separateVoronoiFaces[i].size(); ++j) {

					m_cubeCrossSection.emplace_back(m_separateVoronoiFaces[i][j].a);

				}

			}*/

			for (UINT j = 0; j < m_separateVoronoiFaces[i].size(); ++j) {

				if (m_separateVoronoiFaces[i][j].a.x >= 0.0f && m_separateVoronoiFaces[i][j].a.x <= 1.0f &&
					m_separateVoronoiFaces[i][j].a.y >= 0.0f && m_separateVoronoiFaces[i][j].a.y <= 1.0f &&
					m_separateVoronoiFaces[i][j].a.z >= 0.0f && m_separateVoronoiFaces[i][j].a.z <= 1.0f)
				{

					m_cubeCrossSection.emplace_back(m_separateVoronoiFaces[i][j].a);

				}
			}

			crossSectionCetroidX = 0.0f;
			crossSectionCetroidY = 0.0f;
			crossSectionCetroidZ = 0.0f;

			if (m_cubeCrossSection.size() != 0) {

				//Find centroid of cube cross section points
				for (UINT j = 0; j < m_cubeCrossSection.size(); ++j) {

					crossSectionCetroidX += m_cubeCrossSection[j].x;
					crossSectionCetroidY += m_cubeCrossSection[j].y;
					crossSectionCetroidZ += m_cubeCrossSection[j].z;

				}
				crossSectionCetroidX /= static_cast<FLOAT>(m_cubeCrossSection.size());
				crossSectionCetroidY /= static_cast<FLOAT>(m_cubeCrossSection.size());
				crossSectionCetroidZ /= static_cast<FLOAT>(m_cubeCrossSection.size());

				//Two initial vectors from centroid
				vectorCentroidA = DirectX::XMVectorSet(crossSectionCetroidX - m_cubeCrossSection[0].x, crossSectionCetroidY - m_cubeCrossSection[0].y, crossSectionCetroidZ - m_cubeCrossSection[0].z, 1.0f);
				vectorCentroidB = DirectX::XMVectorSet(crossSectionCetroidX - m_cubeCrossSection[1].x, crossSectionCetroidY - m_cubeCrossSection[1].y, crossSectionCetroidZ - m_cubeCrossSection[1].z, 1.0f);

				vectorCentroidA = DirectX::XMVector3Normalize(vectorCentroidA);
				vectorCentroidB = DirectX::XMVector3Normalize(vectorCentroidB);

				//Calculate angle between the two initial vectors
				vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidB);
				magnitudeA = sqrtf(
					(crossSectionCetroidX - m_cubeCrossSection[0].x) * (crossSectionCetroidX - m_cubeCrossSection[0].x) +
					(crossSectionCetroidY - m_cubeCrossSection[0].y) * (crossSectionCetroidY - m_cubeCrossSection[0].y) +
					(crossSectionCetroidZ - m_cubeCrossSection[0].z) * (crossSectionCetroidZ - m_cubeCrossSection[0].z)
				);
				magnitudeB = sqrtf(
					(crossSectionCetroidX - m_cubeCrossSection[1].x) * (crossSectionCetroidX - m_cubeCrossSection[1].x) +
					(crossSectionCetroidY - m_cubeCrossSection[1].y) * (crossSectionCetroidY - m_cubeCrossSection[1].y) +
					(crossSectionCetroidZ - m_cubeCrossSection[1].z) * (crossSectionCetroidZ - m_cubeCrossSection[1].z)
				);
				//crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct) / (magnitudeA * magnitudeB);
				crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
				if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
				if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
				crossSectionVectorAngle = acosf(crossSectionVectorAngle);
				crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

				//Calculate normal vector for the two initial vectors
				vectorCentroidBNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidB);

				//Calculate normalized vector for one of the initial vectors
				vectorCentroidANormalized = DirectX::XMVector3Normalize(vectorCentroidA);

				//Set the angle for the first two points
				m_cubeCrossSectionCentroidAngle.emplace_back(0.0f);
				m_cubeCrossSectionCentroidAngle.emplace_back(crossSectionVectorAngle);

				//Calculate angle to centroid for each cube cross section point (excluding the first two)
				for (UINT j = 2; j < m_cubeCrossSection.size(); ++j) {

					vectorCentroidX = DirectX::XMVectorSet(
						crossSectionCetroidX - m_cubeCrossSection[j].x,
						crossSectionCetroidY - m_cubeCrossSection[j].y,
						crossSectionCetroidZ - m_cubeCrossSection[j].z,
						1.0f);

					vectorCentroidX = DirectX::XMVector3Normalize(vectorCentroidX);

					//Calculate angle between initial vector and current vector
					vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidX);
					magnitudeA = sqrtf(
						(crossSectionCetroidX - m_cubeCrossSection[0].x) * (crossSectionCetroidX - m_cubeCrossSection[0].x) +
						(crossSectionCetroidY - m_cubeCrossSection[0].y) * (crossSectionCetroidY - m_cubeCrossSection[0].y) +
						(crossSectionCetroidZ - m_cubeCrossSection[0].z) * (crossSectionCetroidZ - m_cubeCrossSection[0].z)
					);
					magnitudeX = sqrtf(
						(crossSectionCetroidX - m_cubeCrossSection[j].x) * (crossSectionCetroidX - m_cubeCrossSection[j].x) +
						(crossSectionCetroidY - m_cubeCrossSection[j].y) * (crossSectionCetroidY - m_cubeCrossSection[j].y) +
						(crossSectionCetroidZ - m_cubeCrossSection[j].z) * (crossSectionCetroidZ - m_cubeCrossSection[j].z)
					);
					//crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct) / (magnitudeA * magnitudeX);
					crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
					if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
					if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
					crossSectionVectorAngle = acosf(crossSectionVectorAngle);
					crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

					//Check for sign

					//Calculate normal vector for initial vector and current vector
					vectorCentroidXNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidX);
					//Calculate dot product of the two normal vectors
					vectorCentroidNormalDotProduct = DirectX::XMVector3Dot(vectorCentroidXNormal, vectorCentroidBNormal);
					angleSign = DirectX::XMVectorGetX(vectorCentroidNormalDotProduct);

					if (angleSign == 0.0f) {

						m_cubeCrossSectionCentroidAngle.emplace_back(180.0f);

					}
					else if (angleSign > 0.0f) {

						m_cubeCrossSectionCentroidAngle.emplace_back(crossSectionVectorAngle);

					}
					else if (angleSign < 0.0f) {

						m_cubeCrossSectionCentroidAngle.emplace_back((180.0f - crossSectionVectorAngle) + 180.0f);

					}

				}

				//Order points
				while (m_cubeCrossSectionCentroidAngle.size() > 0) {

					angleMin = 1000.0f;
					angleMinIndex = 0;

					for (UINT j = 0; j < m_cubeCrossSectionCentroidAngle.size(); ++j) {

						if (m_cubeCrossSectionCentroidAngle[j] < angleMin) {

							angleMin = m_cubeCrossSectionCentroidAngle[j];
							angleMinIndex = j;

						}

					}

					m_cubeCrossSectionOrdered.emplace_back(m_cubeCrossSection[angleMinIndex]);
					m_cubeCrossSectionCentroidAngle.erase(m_cubeCrossSectionCentroidAngle.begin() + angleMinIndex);
					m_cubeCrossSection.erase(m_cubeCrossSection.begin() + angleMinIndex);

				}
				
				//Save in m_separateVoronoiFacesCulledOrdered the ordered points
				for (UINT j = 0; j < m_cubeCrossSectionOrdered.size(); ++j) {

					m_separateVoronoiFacesCulledOrdered[m_separateVoronoiFacesCulledOrdered.size() - 1].emplace_back(m_cubeCrossSectionOrdered[j]);

				}

				//Make triangle list
				for (UINT j = 2; j < m_cubeCrossSectionOrdered.size(); ++j) {

					m_clippedVoronoiTriangles.emplace_back(m_cubeCrossSectionOrdered[0], m_cubeCrossSectionOrdered[j - 1], m_cubeCrossSectionOrdered[j], m_separateVoronoiFaceColors[i]);

				}

			}
			else {

				m_culledVoronoiFacesIndex.emplace_back(i);

			}

		}

	}
	
	//Delete culled voronoi faces from m_separateVoronoiFaces
	for (INT i = static_cast<INT>(m_culledVoronoiFacesIndex.size()) - 1; i >= 0; --i) {

		m_separateVoronoiFaces.erase(m_separateVoronoiFaces.begin() + m_culledVoronoiFacesIndex[i]);

	}

	//Delete culled voronoi faces index from m_pointedVoronoiCells
	for (UINT i = 0; i < m_culledVoronoiFacesIndex.size(); ++i) {

		for (UINT j = 0; j < COMPUTE_SHADER_KC_CENTROID_COUNT; ++j) {

			for (UINT k = 0; k < m_pointedVoronoiCells[j].size(); ++k) {

				if (m_pointedVoronoiCells[j][k] == m_culledVoronoiFacesIndex[i]) {

					m_pointedVoronoiCells[j].erase(m_pointedVoronoiCells[j].begin() + k);

				}

			}

		}

	}

	FLOAT planeSideCentroid = 0.0f;
	FLOAT planeSideUnitCubeVertex = 0.0f;

	FLOAT unitCubeFaceCentroidX = 0.0f;
	FLOAT unitCubeFaceCentroidY = 0.0f;
	FLOAT unitCubeFaceCentroidZ = 0.0f;

	BOOL isDuplicatePoint = FALSE;

	//Construct polygons from the faces of the unit cube
	for (UINT i = 0; i < COMPUTE_SHADER_KC_CENTROID_COUNT; ++i) {

		m_unitCubeVertices.clear();
		m_unitCubeVertices.emplace_back(Point(0.0f, 0.0f, 0.0f));
		m_unitCubeVertices.emplace_back(Point(0.0f, 1.0f, 0.0f));
		m_unitCubeVertices.emplace_back(Point(1.0f, 1.0f, 0.0f));
		m_unitCubeVertices.emplace_back(Point(1.0f, 0.0f, 0.0f));
		m_unitCubeVertices.emplace_back(Point(0.0f, 0.0f, 1.0f));
		m_unitCubeVertices.emplace_back(Point(0.0f, 1.0f, 1.0f));
		m_unitCubeVertices.emplace_back(Point(1.0f, 1.0f, 1.0f));
		m_unitCubeVertices.emplace_back(Point(1.0f, 0.0f, 1.0f));

		m_unitCubeFace1Points.clear();
		m_unitCubeFace2Points.clear();
		m_unitCubeFace3Points.clear();
		m_unitCubeFace4Points.clear();
		m_unitCubeFace5Points.clear();
		m_unitCubeFace6Points.clear();

		//Add points to the m_unitCubeFacePoints' 
		//Check if the unit cube's vertices should be added to the m_unitCubeFacePoints'
		for (UINT j = 0; j < m_pointedVoronoiCells[i].size(); ++j) {
	
			for (UINT k = 0; k < m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]].size(); ++k) {

				if (m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k].z == 0.0f) m_unitCubeFace1Points.emplace_back(m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k]);
				if (m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k].z == 1.0f) m_unitCubeFace2Points.emplace_back(m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k]);
				if (m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k].y == 0.0f) m_unitCubeFace3Points.emplace_back(m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k]);
				if (m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k].y == 1.0f) m_unitCubeFace4Points.emplace_back(m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k]);
				if (m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k].x == 0.0f) m_unitCubeFace5Points.emplace_back(m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k]);
				if (m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k].x == 1.0f) m_unitCubeFace6Points.emplace_back(m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k]);

			}

			//Substitute the centroid into the plane equation
			planeSideCentroid =
				m_separateVoronoiFacePlanes[m_pointedVoronoiCells[i][j]].xCoef * m_centroids[i].x +
				m_separateVoronoiFacePlanes[m_pointedVoronoiCells[i][j]].yCoef * m_centroids[i].y +
				m_separateVoronoiFacePlanes[m_pointedVoronoiCells[i][j]].zCoef * m_centroids[i].z +
				m_separateVoronoiFacePlanes[m_pointedVoronoiCells[i][j]].kCoef;

			//Substitute the unit cube vertices into the plane equation and check if the sign matches planeSideCentroids'
			for (INT k = static_cast<INT>(m_unitCubeVertices.size()) - 1; k >= 0; --k) {

				planeSideUnitCubeVertex =
					m_separateVoronoiFacePlanes[m_pointedVoronoiCells[i][j]].xCoef * m_unitCubeVertices[k].x +
					m_separateVoronoiFacePlanes[m_pointedVoronoiCells[i][j]].yCoef * m_unitCubeVertices[k].y +
					m_separateVoronoiFacePlanes[m_pointedVoronoiCells[i][j]].zCoef * m_unitCubeVertices[k].z +
					m_separateVoronoiFacePlanes[m_pointedVoronoiCells[i][j]].kCoef;

				if ((planeSideCentroid * planeSideUnitCubeVertex) < 0.0f) m_unitCubeVertices.erase(m_unitCubeVertices.begin() + k);

			}

		}

		//Remove duplicates from the m_unitCubeFacePoints
		
		//Face 1.
		for (INT j = static_cast<INT>(m_unitCubeFace1Points.size()) - 1; j >= 1; --j) {

			isDuplicatePoint = FALSE;
			for (INT k = 0; k < j; ++k) {

				if (m_unitCubeFace1Points[j] == m_unitCubeFace1Points[k]) isDuplicatePoint = TRUE;

			}
			if (isDuplicatePoint) m_unitCubeFace1Points.erase(m_unitCubeFace1Points.begin() + j);

		}

		//Face 2.
		for (INT j = static_cast<INT>(m_unitCubeFace2Points.size()) - 1; j >= 1; --j) {

			isDuplicatePoint = FALSE;
			for (INT k = 0; k < j; ++k) {

				if (m_unitCubeFace2Points[j] == m_unitCubeFace2Points[k]) isDuplicatePoint = TRUE;

			}
			if (isDuplicatePoint) m_unitCubeFace2Points.erase(m_unitCubeFace2Points.begin() + j);

		}
		
		//Face 3.
		for (INT j = static_cast<INT>(m_unitCubeFace3Points.size()) - 1; j >= 1; --j) {

			isDuplicatePoint = FALSE;
			for (INT k = 0; k < j; ++k) {

				if (m_unitCubeFace3Points[j] == m_unitCubeFace3Points[k]) isDuplicatePoint = TRUE;

			}
			if (isDuplicatePoint) m_unitCubeFace3Points.erase(m_unitCubeFace3Points.begin() + j);

		}

		//Face 4.
		for (INT j = static_cast<INT>(m_unitCubeFace4Points.size()) - 1; j >= 1; --j) {

			isDuplicatePoint = FALSE;
			for (INT k = 0; k < j; ++k) {

				if (m_unitCubeFace4Points[j] == m_unitCubeFace4Points[k]) isDuplicatePoint = TRUE;

			}
			if (isDuplicatePoint) m_unitCubeFace4Points.erase(m_unitCubeFace4Points.begin() + j);

		}

		//Face 5.
		for (INT j = static_cast<INT>(m_unitCubeFace5Points.size()) - 1; j >= 1; --j) {

			isDuplicatePoint = FALSE;
			for (INT k = 0; k < j; ++k) {

				if (m_unitCubeFace5Points[j] == m_unitCubeFace5Points[k]) isDuplicatePoint = TRUE;

			}
			if (isDuplicatePoint) m_unitCubeFace5Points.erase(m_unitCubeFace5Points.begin() + j);

		}

		//Face 6.
		for (INT j = static_cast<INT>(m_unitCubeFace6Points.size()) - 1; j >= 1; --j) {

			isDuplicatePoint = FALSE;
			for (INT k = 0; k < j; ++k) {

				if (m_unitCubeFace6Points[j] == m_unitCubeFace6Points[k]) isDuplicatePoint = TRUE;

			}
			if (isDuplicatePoint) m_unitCubeFace6Points.erase(m_unitCubeFace6Points.begin() + j);

		}

		//Add unit cube vertices to the m_unitCubeFacePoints' 
		for (UINT j = 0; j < m_unitCubeVertices.size(); ++j) {

			if (m_unitCubeVertices[j].z == 0.0f) m_unitCubeFace1Points.emplace_back(m_unitCubeVertices[j]);
			if (m_unitCubeVertices[j].z == 1.0f) m_unitCubeFace2Points.emplace_back(m_unitCubeVertices[j]);
			if (m_unitCubeVertices[j].y == 0.0f) m_unitCubeFace3Points.emplace_back(m_unitCubeVertices[j]);
			if (m_unitCubeVertices[j].y == 1.0f) m_unitCubeFace4Points.emplace_back(m_unitCubeVertices[j]);
			if (m_unitCubeVertices[j].x == 0.0f) m_unitCubeFace5Points.emplace_back(m_unitCubeVertices[j]);
			if (m_unitCubeVertices[j].x == 1.0f) m_unitCubeFace6Points.emplace_back(m_unitCubeVertices[j]);

		}
		
		//Construct polygons

		//Unit cube face 1.
		if (m_unitCubeFace1Points.size() > 0) {

			m_unitCubeFaceCentroidAngle.clear();
			m_unitCubeFaceOrdered.clear();

			unitCubeFaceCentroidX = 0.0f;
			unitCubeFaceCentroidY = 0.0f;
			unitCubeFaceCentroidZ = 0.0f;

			colorR = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorG = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorB = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;

			//Find centroid of polygon face points
			{

				for (UINT j = 0; j < m_unitCubeFace1Points.size(); ++j) {

					unitCubeFaceCentroidX += m_unitCubeFace1Points[j].x;
					unitCubeFaceCentroidY += m_unitCubeFace1Points[j].y;
					unitCubeFaceCentroidZ += m_unitCubeFace1Points[j].z;

				}
				unitCubeFaceCentroidX /= static_cast<FLOAT>(m_unitCubeFace1Points.size());
				unitCubeFaceCentroidY /= static_cast<FLOAT>(m_unitCubeFace1Points.size());
				unitCubeFaceCentroidZ /= static_cast<FLOAT>(m_unitCubeFace1Points.size());

			}

			//Calculate angle of the first two points
			vectorCentroidA = DirectX::XMVectorSet(unitCubeFaceCentroidX - m_unitCubeFace1Points[0].x, unitCubeFaceCentroidY - m_unitCubeFace1Points[0].y, unitCubeFaceCentroidZ - m_unitCubeFace1Points[0].z, 1.0f);
			vectorCentroidB = DirectX::XMVectorSet(unitCubeFaceCentroidX - m_unitCubeFace1Points[1].x, unitCubeFaceCentroidY - m_unitCubeFace1Points[1].y, unitCubeFaceCentroidZ - m_unitCubeFace1Points[1].z, 1.0f);

			vectorCentroidA = DirectX::XMVector3Normalize(vectorCentroidA);
			vectorCentroidB = DirectX::XMVector3Normalize(vectorCentroidB);

			//Calculate angle between the two initial vectors
			vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidB);
			crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
			if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
			if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
			crossSectionVectorAngle = acosf(crossSectionVectorAngle);
			crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

			//Calculate normal vector for the two initial vectors
			vectorCentroidBNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidB);

			m_unitCubeFaceCentroidAngle.emplace_back(0.0f);
			m_unitCubeFaceCentroidAngle.emplace_back(crossSectionVectorAngle);

			//Calculate angle for the rest of the points
			for (UINT j = 2; j < m_unitCubeFace1Points.size(); ++j) {

				vectorCentroidX = DirectX::XMVectorSet(
					unitCubeFaceCentroidX - m_unitCubeFace1Points[j].x,
					unitCubeFaceCentroidY - m_unitCubeFace1Points[j].y,
					unitCubeFaceCentroidZ - m_unitCubeFace1Points[j].z,
					1.0f);

				vectorCentroidX = DirectX::XMVector3Normalize(vectorCentroidX);

				//Calculate angle between initial vector and current vector
				vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidX);
				crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
				if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
				if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
				crossSectionVectorAngle = acosf(crossSectionVectorAngle);
				crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

				//Check for sign

				//Calculate normal vector for initial vector and current vector
				vectorCentroidXNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidX);
				//Calculate dot product of the two normal vectors
				vectorCentroidNormalDotProduct = DirectX::XMVector3Dot(vectorCentroidXNormal, vectorCentroidBNormal);
				angleSign = DirectX::XMVectorGetX(vectorCentroidNormalDotProduct);

				if (angleSign == 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back(180.0f);

				}
				else if (angleSign > 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back(crossSectionVectorAngle);

				}
				else if (angleSign < 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back((180.0f - crossSectionVectorAngle) + 180.0f);

				}

			}

			//Order points
			while (m_unitCubeFaceCentroidAngle.size() > 0) {

				angleMin = 1000.0f;
				angleMinIndex = 0;

				for (UINT j = 0; j < m_unitCubeFaceCentroidAngle.size(); ++j) {

					if (m_unitCubeFaceCentroidAngle[j] < angleMin) {

						angleMin = m_unitCubeFaceCentroidAngle[j];
						angleMinIndex = j;

					}

				}

				m_unitCubeFaceOrdered.emplace_back(m_unitCubeFace1Points[angleMinIndex]);
				m_unitCubeFaceCentroidAngle.erase(m_unitCubeFaceCentroidAngle.begin() + angleMinIndex);
				m_unitCubeFace1Points.erase(m_unitCubeFace1Points.begin() + angleMinIndex);

			}

			//Store the face in m_separateVoronoiFacesCulledOrdered
			m_separateVoronoiFacesCulledOrdered.emplace_back();
			for (UINT j = 0; j < m_unitCubeFaceOrdered.size(); ++j) {

				m_separateVoronoiFacesCulledOrdered[m_separateVoronoiFacesCulledOrdered.size() - 1].emplace_back(m_unitCubeFaceOrdered[j]);

			}
			m_pointedVoronoiCells[i].emplace_back(static_cast<UINT>(m_separateVoronoiFacesCulledOrdered.size() - 1));
			m_separateVoronoiFaceColors.emplace_back(colorR, colorG, colorB);

		}

		//Unit cube face 2.
		if (m_unitCubeFace2Points.size() > 0) {

			m_unitCubeFaceCentroidAngle.clear();
			m_unitCubeFaceOrdered.clear();

			unitCubeFaceCentroidX = 0.0f;
			unitCubeFaceCentroidY = 0.0f;
			unitCubeFaceCentroidZ = 0.0f;

			colorR = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorG = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorB = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;

			//Find centroid of polygon face points
			{

				for (UINT j = 0; j < m_unitCubeFace2Points.size(); ++j) {

					unitCubeFaceCentroidX += m_unitCubeFace2Points[j].x;
					unitCubeFaceCentroidY += m_unitCubeFace2Points[j].y;
					unitCubeFaceCentroidZ += m_unitCubeFace2Points[j].z;

				}
				unitCubeFaceCentroidX /= static_cast<FLOAT>(m_unitCubeFace2Points.size());
				unitCubeFaceCentroidY /= static_cast<FLOAT>(m_unitCubeFace2Points.size());
				unitCubeFaceCentroidZ /= static_cast<FLOAT>(m_unitCubeFace2Points.size());

			}

			//Calculate angle of the first two points
			vectorCentroidA = DirectX::XMVectorSet(unitCubeFaceCentroidX - m_unitCubeFace2Points[0].x, unitCubeFaceCentroidY - m_unitCubeFace2Points[0].y, unitCubeFaceCentroidZ - m_unitCubeFace2Points[0].z, 1.0f);
			vectorCentroidB = DirectX::XMVectorSet(unitCubeFaceCentroidX - m_unitCubeFace2Points[1].x, unitCubeFaceCentroidY - m_unitCubeFace2Points[1].y, unitCubeFaceCentroidZ - m_unitCubeFace2Points[1].z, 1.0f);

			vectorCentroidA = DirectX::XMVector3Normalize(vectorCentroidA);
			vectorCentroidB = DirectX::XMVector3Normalize(vectorCentroidB);

			//Calculate angle between the two initial vectors
			vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidB);
			crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
			if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
			if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
			crossSectionVectorAngle = acosf(crossSectionVectorAngle);
			crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

			//Calculate normal vector for the two initial vectors
			vectorCentroidBNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidB);

			m_unitCubeFaceCentroidAngle.emplace_back(0.0f);
			m_unitCubeFaceCentroidAngle.emplace_back(crossSectionVectorAngle);

			//Calculate angle for the rest of the points
			for (UINT j = 2; j < m_unitCubeFace2Points.size(); ++j) {

				vectorCentroidX = DirectX::XMVectorSet(
					unitCubeFaceCentroidX - m_unitCubeFace2Points[j].x,
					unitCubeFaceCentroidY - m_unitCubeFace2Points[j].y,
					unitCubeFaceCentroidZ - m_unitCubeFace2Points[j].z,
					1.0f);

				vectorCentroidX = DirectX::XMVector3Normalize(vectorCentroidX);

				//Calculate angle between initial vector and current vector
				vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidX);
				crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
				if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
				if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
				crossSectionVectorAngle = acosf(crossSectionVectorAngle);
				crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

				//Check for sign

				//Calculate normal vector for initial vector and current vector
				vectorCentroidXNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidX);
				//Calculate dot product of the two normal vectors
				vectorCentroidNormalDotProduct = DirectX::XMVector3Dot(vectorCentroidXNormal, vectorCentroidBNormal);
				angleSign = DirectX::XMVectorGetX(vectorCentroidNormalDotProduct);

				if (angleSign == 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back(180.0f);

				}
				else if (angleSign > 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back(crossSectionVectorAngle);

				}
				else if (angleSign < 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back((180.0f - crossSectionVectorAngle) + 180.0f);

				}

			}

			//Order points
			while (m_unitCubeFaceCentroidAngle.size() > 0) {

				angleMin = 1000.0f;
				angleMinIndex = 0;

				for (UINT j = 0; j < m_unitCubeFaceCentroidAngle.size(); ++j) {

					if (m_unitCubeFaceCentroidAngle[j] < angleMin) {

						angleMin = m_unitCubeFaceCentroidAngle[j];
						angleMinIndex = j;

					}

				}

				m_unitCubeFaceOrdered.emplace_back(m_unitCubeFace2Points[angleMinIndex]);
				m_unitCubeFaceCentroidAngle.erase(m_unitCubeFaceCentroidAngle.begin() + angleMinIndex);
				m_unitCubeFace2Points.erase(m_unitCubeFace2Points.begin() + angleMinIndex);

			}

			//Store the face in m_separateVoronoiFacesCulledOrdered
			m_separateVoronoiFacesCulledOrdered.emplace_back();
			for (UINT j = 0; j < m_unitCubeFaceOrdered.size(); ++j) {

				m_separateVoronoiFacesCulledOrdered[m_separateVoronoiFacesCulledOrdered.size() - 1].emplace_back(m_unitCubeFaceOrdered[j]);

			}
			m_pointedVoronoiCells[i].emplace_back(static_cast<UINT>(m_separateVoronoiFacesCulledOrdered.size() - 1));
			m_separateVoronoiFaceColors.emplace_back(colorR, colorG, colorB);

		}

		//Unit cube face 3.
		if (m_unitCubeFace3Points.size() > 0) {

			m_unitCubeFaceCentroidAngle.clear();
			m_unitCubeFaceOrdered.clear();

			unitCubeFaceCentroidX = 0.0f;
			unitCubeFaceCentroidY = 0.0f;
			unitCubeFaceCentroidZ = 0.0f;

			colorR = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorG = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorB = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;

			//Find centroid of polygon face points
			{

				for (UINT j = 0; j < m_unitCubeFace3Points.size(); ++j) {

					unitCubeFaceCentroidX += m_unitCubeFace3Points[j].x;
					unitCubeFaceCentroidY += m_unitCubeFace3Points[j].y;
					unitCubeFaceCentroidZ += m_unitCubeFace3Points[j].z;

				}
				unitCubeFaceCentroidX /= static_cast<FLOAT>(m_unitCubeFace3Points.size());
				unitCubeFaceCentroidY /= static_cast<FLOAT>(m_unitCubeFace3Points.size());
				unitCubeFaceCentroidZ /= static_cast<FLOAT>(m_unitCubeFace3Points.size());

			}

			//Calculate angle of the first two points
			vectorCentroidA = DirectX::XMVectorSet(unitCubeFaceCentroidX - m_unitCubeFace3Points[0].x, unitCubeFaceCentroidY - m_unitCubeFace3Points[0].y, unitCubeFaceCentroidZ - m_unitCubeFace3Points[0].z, 1.0f);
			vectorCentroidB = DirectX::XMVectorSet(unitCubeFaceCentroidX - m_unitCubeFace3Points[1].x, unitCubeFaceCentroidY - m_unitCubeFace3Points[1].y, unitCubeFaceCentroidZ - m_unitCubeFace3Points[1].z, 1.0f);

			vectorCentroidA = DirectX::XMVector3Normalize(vectorCentroidA);
			vectorCentroidB = DirectX::XMVector3Normalize(vectorCentroidB);

			//Calculate angle between the two initial vectors
			vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidB);
			crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
			if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
			if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
			crossSectionVectorAngle = acosf(crossSectionVectorAngle);
			crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

			//Calculate normal vector for the two initial vectors
			vectorCentroidBNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidB);

			m_unitCubeFaceCentroidAngle.emplace_back(0.0f);
			m_unitCubeFaceCentroidAngle.emplace_back(crossSectionVectorAngle);

			//Calculate angle for the rest of the points
			for (UINT j = 2; j < m_unitCubeFace3Points.size(); ++j) {

				vectorCentroidX = DirectX::XMVectorSet(
					unitCubeFaceCentroidX - m_unitCubeFace3Points[j].x,
					unitCubeFaceCentroidY - m_unitCubeFace3Points[j].y,
					unitCubeFaceCentroidZ - m_unitCubeFace3Points[j].z,
					1.0f);

				vectorCentroidX = DirectX::XMVector3Normalize(vectorCentroidX);

				//Calculate angle between initial vector and current vector
				vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidX);
				crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
				if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
				if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
				crossSectionVectorAngle = acosf(crossSectionVectorAngle);
				crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

				//Check for sign

				//Calculate normal vector for initial vector and current vector
				vectorCentroidXNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidX);
				//Calculate dot product of the two normal vectors
				vectorCentroidNormalDotProduct = DirectX::XMVector3Dot(vectorCentroidXNormal, vectorCentroidBNormal);
				angleSign = DirectX::XMVectorGetX(vectorCentroidNormalDotProduct);

				if (angleSign == 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back(180.0f);

				}
				else if (angleSign > 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back(crossSectionVectorAngle);

				}
				else if (angleSign < 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back((180.0f - crossSectionVectorAngle) + 180.0f);

				}

			}

			//Order points
			while (m_unitCubeFaceCentroidAngle.size() > 0) {

				angleMin = 1000.0f;
				angleMinIndex = 0;

				for (UINT j = 0; j < m_unitCubeFaceCentroidAngle.size(); ++j) {

					if (m_unitCubeFaceCentroidAngle[j] < angleMin) {

						angleMin = m_unitCubeFaceCentroidAngle[j];
						angleMinIndex = j;

					}

				}

				m_unitCubeFaceOrdered.emplace_back(m_unitCubeFace3Points[angleMinIndex]);
				m_unitCubeFaceCentroidAngle.erase(m_unitCubeFaceCentroidAngle.begin() + angleMinIndex);
				m_unitCubeFace3Points.erase(m_unitCubeFace3Points.begin() + angleMinIndex);

			}

			//Store the face in m_separateVoronoiFacesCulledOrdered
			m_separateVoronoiFacesCulledOrdered.emplace_back();
			for (UINT j = 0; j < m_unitCubeFaceOrdered.size(); ++j) {

				m_separateVoronoiFacesCulledOrdered[m_separateVoronoiFacesCulledOrdered.size() - 1].emplace_back(m_unitCubeFaceOrdered[j]);

			}
			m_pointedVoronoiCells[i].emplace_back(static_cast<UINT>(m_separateVoronoiFacesCulledOrdered.size() - 1));
			m_separateVoronoiFaceColors.emplace_back(colorR, colorG, colorB);

		}

		//Unit cube face 4.
		if (m_unitCubeFace4Points.size() > 0) {

			m_unitCubeFaceCentroidAngle.clear();
			m_unitCubeFaceOrdered.clear();

			unitCubeFaceCentroidX = 0.0f;
			unitCubeFaceCentroidY = 0.0f;
			unitCubeFaceCentroidZ = 0.0f;

			colorR = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorG = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorB = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;

			//Find centroid of polygon face points
			{

				for (UINT j = 0; j < m_unitCubeFace4Points.size(); ++j) {

					unitCubeFaceCentroidX += m_unitCubeFace4Points[j].x;
					unitCubeFaceCentroidY += m_unitCubeFace4Points[j].y;
					unitCubeFaceCentroidZ += m_unitCubeFace4Points[j].z;

				}
				unitCubeFaceCentroidX /= static_cast<FLOAT>(m_unitCubeFace4Points.size());
				unitCubeFaceCentroidY /= static_cast<FLOAT>(m_unitCubeFace4Points.size());
				unitCubeFaceCentroidZ /= static_cast<FLOAT>(m_unitCubeFace4Points.size());

			}

			//Calculate angle of the first two points
			vectorCentroidA = DirectX::XMVectorSet(unitCubeFaceCentroidX - m_unitCubeFace4Points[0].x, unitCubeFaceCentroidY - m_unitCubeFace4Points[0].y, unitCubeFaceCentroidZ - m_unitCubeFace4Points[0].z, 1.0f);
			vectorCentroidB = DirectX::XMVectorSet(unitCubeFaceCentroidX - m_unitCubeFace4Points[1].x, unitCubeFaceCentroidY - m_unitCubeFace4Points[1].y, unitCubeFaceCentroidZ - m_unitCubeFace4Points[1].z, 1.0f);

			vectorCentroidA = DirectX::XMVector3Normalize(vectorCentroidA);
			vectorCentroidB = DirectX::XMVector3Normalize(vectorCentroidB);

			//Calculate angle between the two initial vectors
			vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidB);
			crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
			if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
			if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
			crossSectionVectorAngle = acosf(crossSectionVectorAngle);
			crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

			//Calculate normal vector for the two initial vectors
			vectorCentroidBNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidB);

			m_unitCubeFaceCentroidAngle.emplace_back(0.0f);
			m_unitCubeFaceCentroidAngle.emplace_back(crossSectionVectorAngle);

			//Calculate angle for the rest of the points
			for (UINT j = 2; j < m_unitCubeFace4Points.size(); ++j) {

				vectorCentroidX = DirectX::XMVectorSet(
					unitCubeFaceCentroidX - m_unitCubeFace4Points[j].x,
					unitCubeFaceCentroidY - m_unitCubeFace4Points[j].y,
					unitCubeFaceCentroidZ - m_unitCubeFace4Points[j].z,
					1.0f);

				vectorCentroidX = DirectX::XMVector3Normalize(vectorCentroidX);

				//Calculate angle between initial vector and current vector
				vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidX);
				crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
				if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
				if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
				crossSectionVectorAngle = acosf(crossSectionVectorAngle);
				crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

				//Check for sign

				//Calculate normal vector for initial vector and current vector
				vectorCentroidXNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidX);
				//Calculate dot product of the two normal vectors
				vectorCentroidNormalDotProduct = DirectX::XMVector3Dot(vectorCentroidXNormal, vectorCentroidBNormal);
				angleSign = DirectX::XMVectorGetX(vectorCentroidNormalDotProduct);

				if (angleSign == 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back(180.0f);

				}
				else if (angleSign > 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back(crossSectionVectorAngle);

				}
				else if (angleSign < 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back((180.0f - crossSectionVectorAngle) + 180.0f);

				}

			}

			//Order points
			while (m_unitCubeFaceCentroidAngle.size() > 0) {

				angleMin = 1000.0f;
				angleMinIndex = 0;

				for (UINT j = 0; j < m_unitCubeFaceCentroidAngle.size(); ++j) {

					if (m_unitCubeFaceCentroidAngle[j] < angleMin) {

						angleMin = m_unitCubeFaceCentroidAngle[j];
						angleMinIndex = j;

					}

				}

				m_unitCubeFaceOrdered.emplace_back(m_unitCubeFace4Points[angleMinIndex]);
				m_unitCubeFaceCentroidAngle.erase(m_unitCubeFaceCentroidAngle.begin() + angleMinIndex);
				m_unitCubeFace4Points.erase(m_unitCubeFace4Points.begin() + angleMinIndex);

			}

			//Store the face in m_separateVoronoiFacesCulledOrdered
			m_separateVoronoiFacesCulledOrdered.emplace_back();
			for (UINT j = 0; j < m_unitCubeFaceOrdered.size(); ++j) {

				m_separateVoronoiFacesCulledOrdered[m_separateVoronoiFacesCulledOrdered.size() - 1].emplace_back(m_unitCubeFaceOrdered[j]);

			}
			m_pointedVoronoiCells[i].emplace_back(static_cast<UINT>(m_separateVoronoiFacesCulledOrdered.size() - 1));
			m_separateVoronoiFaceColors.emplace_back(colorR, colorG, colorB);

		}

		//Unit cube face 5.
		if (m_unitCubeFace5Points.size() > 0) {

			m_unitCubeFaceCentroidAngle.clear();
			m_unitCubeFaceOrdered.clear();

			unitCubeFaceCentroidX = 0.0f;
			unitCubeFaceCentroidY = 0.0f;
			unitCubeFaceCentroidZ = 0.0f;

			colorR = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorG = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorB = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;

			//Find centroid of polygon face points
			{

				for (UINT j = 0; j < m_unitCubeFace5Points.size(); ++j) {

					unitCubeFaceCentroidX += m_unitCubeFace5Points[j].x;
					unitCubeFaceCentroidY += m_unitCubeFace5Points[j].y;
					unitCubeFaceCentroidZ += m_unitCubeFace5Points[j].z;

				}
				unitCubeFaceCentroidX /= static_cast<FLOAT>(m_unitCubeFace5Points.size());
				unitCubeFaceCentroidY /= static_cast<FLOAT>(m_unitCubeFace5Points.size());
				unitCubeFaceCentroidZ /= static_cast<FLOAT>(m_unitCubeFace5Points.size());

			}

			//Calculate angle of the first two points
			vectorCentroidA = DirectX::XMVectorSet(unitCubeFaceCentroidX - m_unitCubeFace5Points[0].x, unitCubeFaceCentroidY - m_unitCubeFace5Points[0].y, unitCubeFaceCentroidZ - m_unitCubeFace5Points[0].z, 1.0f);
			vectorCentroidB = DirectX::XMVectorSet(unitCubeFaceCentroidX - m_unitCubeFace5Points[1].x, unitCubeFaceCentroidY - m_unitCubeFace5Points[1].y, unitCubeFaceCentroidZ - m_unitCubeFace5Points[1].z, 1.0f);

			vectorCentroidA = DirectX::XMVector3Normalize(vectorCentroidA);
			vectorCentroidB = DirectX::XMVector3Normalize(vectorCentroidB);

			//Calculate angle between the two initial vectors
			vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidB);
			crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
			if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
			if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
			crossSectionVectorAngle = acosf(crossSectionVectorAngle);
			crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

			//Calculate normal vector for the two initial vectors
			vectorCentroidBNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidB);

			m_unitCubeFaceCentroidAngle.emplace_back(0.0f);
			m_unitCubeFaceCentroidAngle.emplace_back(crossSectionVectorAngle);

			//Calculate angle for the rest of the points
			for (UINT j = 2; j < m_unitCubeFace5Points.size(); ++j) {

				vectorCentroidX = DirectX::XMVectorSet(
					unitCubeFaceCentroidX - m_unitCubeFace5Points[j].x,
					unitCubeFaceCentroidY - m_unitCubeFace5Points[j].y,
					unitCubeFaceCentroidZ - m_unitCubeFace5Points[j].z,
					1.0f);

				vectorCentroidX = DirectX::XMVector3Normalize(vectorCentroidX);

				//Calculate angle between initial vector and current vector
				vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidX);
				crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
				if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
				if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
				crossSectionVectorAngle = acosf(crossSectionVectorAngle);
				crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

				//Check for sign

				//Calculate normal vector for initial vector and current vector
				vectorCentroidXNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidX);
				//Calculate dot product of the two normal vectors
				vectorCentroidNormalDotProduct = DirectX::XMVector3Dot(vectorCentroidXNormal, vectorCentroidBNormal);
				angleSign = DirectX::XMVectorGetX(vectorCentroidNormalDotProduct);

				if (angleSign == 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back(180.0f);

				}
				else if (angleSign > 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back(crossSectionVectorAngle);

				}
				else if (angleSign < 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back((180.0f - crossSectionVectorAngle) + 180.0f);

				}

			}

			//Order points
			while (m_unitCubeFaceCentroidAngle.size() > 0) {

				angleMin = 1000.0f;
				angleMinIndex = 0;

				for (UINT j = 0; j < m_unitCubeFaceCentroidAngle.size(); ++j) {

					if (m_unitCubeFaceCentroidAngle[j] < angleMin) {

						angleMin = m_unitCubeFaceCentroidAngle[j];
						angleMinIndex = j;

					}

				}

				m_unitCubeFaceOrdered.emplace_back(m_unitCubeFace5Points[angleMinIndex]);
				m_unitCubeFaceCentroidAngle.erase(m_unitCubeFaceCentroidAngle.begin() + angleMinIndex);
				m_unitCubeFace5Points.erase(m_unitCubeFace5Points.begin() + angleMinIndex);

			}

			//Store the face in m_separateVoronoiFacesCulledOrdered
			m_separateVoronoiFacesCulledOrdered.emplace_back();
			for (UINT j = 0; j < m_unitCubeFaceOrdered.size(); ++j) {

				m_separateVoronoiFacesCulledOrdered[m_separateVoronoiFacesCulledOrdered.size() - 1].emplace_back(m_unitCubeFaceOrdered[j]);

			}
			m_pointedVoronoiCells[i].emplace_back(static_cast<UINT>(m_separateVoronoiFacesCulledOrdered.size() - 1));
			m_separateVoronoiFaceColors.emplace_back(colorR, colorG, colorB);

		}

		//Unit cube face 6.
		if (m_unitCubeFace6Points.size() > 0) {

			m_unitCubeFaceCentroidAngle.clear();
			m_unitCubeFaceOrdered.clear();

			unitCubeFaceCentroidX = 0.0f;
			unitCubeFaceCentroidY = 0.0f;
			unitCubeFaceCentroidZ = 0.0f;

			colorR = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorG = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
			colorB = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;

			//Find centroid of polygon face points
			{

				for (UINT j = 0; j < m_unitCubeFace6Points.size(); ++j) {

					unitCubeFaceCentroidX += m_unitCubeFace6Points[j].x;
					unitCubeFaceCentroidY += m_unitCubeFace6Points[j].y;
					unitCubeFaceCentroidZ += m_unitCubeFace6Points[j].z;

				}
				unitCubeFaceCentroidX /= static_cast<FLOAT>(m_unitCubeFace6Points.size());
				unitCubeFaceCentroidY /= static_cast<FLOAT>(m_unitCubeFace6Points.size());
				unitCubeFaceCentroidZ /= static_cast<FLOAT>(m_unitCubeFace6Points.size());

			}

			//Calculate angle of the first two points
			vectorCentroidA = DirectX::XMVectorSet(unitCubeFaceCentroidX - m_unitCubeFace6Points[0].x, unitCubeFaceCentroidY - m_unitCubeFace6Points[0].y, unitCubeFaceCentroidZ - m_unitCubeFace6Points[0].z, 1.0f);
			vectorCentroidB = DirectX::XMVectorSet(unitCubeFaceCentroidX - m_unitCubeFace6Points[1].x, unitCubeFaceCentroidY - m_unitCubeFace6Points[1].y, unitCubeFaceCentroidZ - m_unitCubeFace6Points[1].z, 1.0f);

			vectorCentroidA = DirectX::XMVector3Normalize(vectorCentroidA);
			vectorCentroidB = DirectX::XMVector3Normalize(vectorCentroidB);

			//Calculate angle between the two initial vectors
			vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidB);
			crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
			if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
			if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
			crossSectionVectorAngle = acosf(crossSectionVectorAngle);
			crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

			//Calculate normal vector for the two initial vectors
			vectorCentroidBNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidB);

			m_unitCubeFaceCentroidAngle.emplace_back(0.0f);
			m_unitCubeFaceCentroidAngle.emplace_back(crossSectionVectorAngle);

			//Calculate angle for the rest of the points
			for (UINT j = 2; j < m_unitCubeFace6Points.size(); ++j) {

				vectorCentroidX = DirectX::XMVectorSet(
					unitCubeFaceCentroidX - m_unitCubeFace6Points[j].x,
					unitCubeFaceCentroidY - m_unitCubeFace6Points[j].y,
					unitCubeFaceCentroidZ - m_unitCubeFace6Points[j].z,
					1.0f);

				vectorCentroidX = DirectX::XMVector3Normalize(vectorCentroidX);

				//Calculate angle between initial vector and current vector
				vectorCentroidDotProduct = DirectX::XMVector3Dot(vectorCentroidA, vectorCentroidX);
				crossSectionVectorAngle = DirectX::XMVectorGetX(vectorCentroidDotProduct);
				if (crossSectionVectorAngle > 1.0f) crossSectionVectorAngle = 1.0f;
				if (crossSectionVectorAngle < -1.0f) crossSectionVectorAngle = -1.0f;
				crossSectionVectorAngle = acosf(crossSectionVectorAngle);
				crossSectionVectorAngle = DirectX::XMConvertToDegrees(crossSectionVectorAngle);

				//Check for sign

				//Calculate normal vector for initial vector and current vector
				vectorCentroidXNormal = DirectX::XMVector3Cross(vectorCentroidA, vectorCentroidX);
				//Calculate dot product of the two normal vectors
				vectorCentroidNormalDotProduct = DirectX::XMVector3Dot(vectorCentroidXNormal, vectorCentroidBNormal);
				angleSign = DirectX::XMVectorGetX(vectorCentroidNormalDotProduct);

				if (angleSign == 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back(180.0f);

				}
				else if (angleSign > 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back(crossSectionVectorAngle);

				}
				else if (angleSign < 0.0f) {

					m_unitCubeFaceCentroidAngle.emplace_back((180.0f - crossSectionVectorAngle) + 180.0f);

				}

			}

			//Order points
			while (m_unitCubeFaceCentroidAngle.size() > 0) {

				angleMin = 1000.0f;
				angleMinIndex = 0;

				for (UINT j = 0; j < m_unitCubeFaceCentroidAngle.size(); ++j) {

					if (m_unitCubeFaceCentroidAngle[j] < angleMin) {

						angleMin = m_unitCubeFaceCentroidAngle[j];
						angleMinIndex = j;

					}

				}

				m_unitCubeFaceOrdered.emplace_back(m_unitCubeFace6Points[angleMinIndex]);
				m_unitCubeFaceCentroidAngle.erase(m_unitCubeFaceCentroidAngle.begin() + angleMinIndex);
				m_unitCubeFace6Points.erase(m_unitCubeFace6Points.begin() + angleMinIndex);

			}

			//Store the face in m_separateVoronoiFacesCulledOrdered
			m_separateVoronoiFacesCulledOrdered.emplace_back();
			for (UINT j = 0; j < m_unitCubeFaceOrdered.size(); ++j) {

				m_separateVoronoiFacesCulledOrdered[m_separateVoronoiFacesCulledOrdered.size() - 1].emplace_back(m_unitCubeFaceOrdered[j]);

			}
			m_pointedVoronoiCells[i].emplace_back(static_cast<UINT>(m_separateVoronoiFacesCulledOrdered.size() - 1));
			m_separateVoronoiFaceColors.emplace_back(colorR, colorG, colorB);

		}

	}

	Point clippedVoronoiFacesCentroid = {};
	FLOAT clippedScaleConstant = 1.0f;

	//Construct triangles from m_separateVoronoiFacesCulledOrdered for m_clippedVoronoiCellsTriangles
	for (UINT i = 0; i < COMPUTE_SHADER_KC_CENTROID_COUNT; ++i) {

		clippedVoronoiFacesCentroid = Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z);

		for (UINT j = 0; j < m_pointedVoronoiCells[i].size(); ++j) {

			for (INT k = 2; k < m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]].size(); ++k) {
		
				centeredPointA = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][0];
				centeredPointB = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k - 1];
				centeredPointC = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k];

				centeredPointA -= clippedVoronoiFacesCentroid;
				centeredPointB -= clippedVoronoiFacesCentroid;
				centeredPointC -= clippedVoronoiFacesCentroid;

				centeredPointA *= clippedScaleConstant;
				centeredPointB *= clippedScaleConstant;
				centeredPointC *= clippedScaleConstant;

				centeredPointA += clippedVoronoiFacesCentroid;
				centeredPointB += clippedVoronoiFacesCentroid;
				centeredPointC += clippedVoronoiFacesCentroid;

				m_clippedVoronoiCellsTriangles.emplace_back(centeredPointA, centeredPointB, centeredPointC, m_separateVoronoiFaceColors[m_pointedVoronoiCells[i][j]]);

			} 
		
		}

	}

	FLOAT planeNormalX = 0.0f;
	FLOAT planeNormalY = 0.0f;
	FLOAT planeNormalZ = 0.0f;
	DirectX::XMVECTOR vectorPlaneNormalDotProduct = {};
	FLOAT planeNormalDotProduct = 0.0f;

	//Calculate face normals (normalized) for m_separateVoronoiFacesCulledOrdered
	for (UINT i = 0; i < m_separateVoronoiFacesCulledOrdered.size(); ++i) {

		if (m_separateVoronoiFacesCulledOrdered[i].size() == 0) {

			m_separateVoronoiFaceNormals.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);

		}
		else {

			//Three point on the plane
			planePointA = m_separateVoronoiFacesCulledOrdered[i][0];
			planePointB = m_separateVoronoiFacesCulledOrdered[i][1];
			planePointC = m_separateVoronoiFacesCulledOrdered[i][2];

			//Vectors A - B; B - C
			vectorAB = DirectX::XMVectorSet(planePointA.x - planePointB.x, planePointA.y - planePointB.y, planePointA.z - planePointB.z, 1.0f);
			vectorBC = DirectX::XMVectorSet(planePointB.x - planePointC.x, planePointB.y - planePointC.y, planePointB.z - planePointC.z, 1.0f);

			//Cross product of AB x BC
			planeNormal = DirectX::XMVector3Cross(vectorAB, vectorBC);

			//Check if 0.5f, 0.5f, 0.5f (unit cube center) is in the direction of the normal
			vectorAB = DirectX::XMVectorSet(0.5f - planePointA.x, 0.5f - planePointA.y, 0.5f - planePointA.z, 1.0f);
			vectorPlaneNormalDotProduct = DirectX::XMVector3Dot(vectorAB, planeNormal);
			planeNormalDotProduct = DirectX::XMVectorGetX(vectorPlaneNormalDotProduct);

			//Normalize the vector
			planeNormal = DirectX::XMVector3Normalize(planeNormal);

			if (planeNormalDotProduct < 0.0f) {

				planeNormalX = DirectX::XMVectorGetX(planeNormal);
				planeNormalY = DirectX::XMVectorGetY(planeNormal);
				planeNormalZ = DirectX::XMVectorGetZ(planeNormal);

			}
			else {

				planeNormalX = -DirectX::XMVectorGetX(planeNormal);
				planeNormalY = -DirectX::XMVectorGetY(planeNormal);
				planeNormalZ = -DirectX::XMVectorGetZ(planeNormal);


			}
			
			m_separateVoronoiFaceNormals.emplace_back(planeNormalX, planeNormalY, planeNormalZ, 0.0f);

		}

	}

	//Sienna Brown
	Point renderedCubeColor(160.0f / 255.0f, 82.0f / 255.0f, 45.0f / 255.0f);
	
	//Construct triangles from m_separateVoronoiFacesCulledOrdered for m_clippedVoronoiTrianglesFaceNormals
	for (UINT i = 0; i < m_separateVoronoiFacesCulledOrdered.size(); ++i) {
		
		if (m_separateVoronoiFacesCulledOrdered[i].size() > 0) {

			for (INT j = 2; j < m_separateVoronoiFacesCulledOrdered[i].size(); ++j) {

				m_clippedVoronoiTrianglesFaceNormals.emplace_back(
					m_separateVoronoiFacesCulledOrdered[i][0], 
					m_separateVoronoiFacesCulledOrdered[i][j - 1], 
					m_separateVoronoiFacesCulledOrdered[i][j],
					renderedCubeColor,
					Point(m_separateVoronoiFaceNormals[i].xCoef, m_separateVoronoiFaceNormals[i].yCoef, m_separateVoronoiFaceNormals[i].zCoef)
				);

			}

		}

	}
	
	Point whiteEdgeColor(1.0f, 1.0f, 1.0f);
	//Construct voronoi edges that lie on the unit cube
	for (UINT i = 0; i < m_separateVoronoiFacesCulledOrdered.size(); ++i) {
	
		if (m_separateVoronoiFacesCulledOrdered[i].size() > 0) {

			for (INT j = 1; j < m_separateVoronoiFacesCulledOrdered[i].size(); ++j) {

				m_voronoiEdgesUnitCube.emplace_back(
					m_separateVoronoiFacesCulledOrdered[i][j - 1],
					m_separateVoronoiFacesCulledOrdered[i][j],
					whiteEdgeColor);

			}

			m_voronoiEdgesUnitCube.emplace_back(
				m_separateVoronoiFacesCulledOrdered[i][m_separateVoronoiFacesCulledOrdered[i].size() - 1],
				m_separateVoronoiFacesCulledOrdered[i][0],
				whiteEdgeColor
			);

		}
	
	}

	DirectX::XMVECTOR surfaceNormal = {};
	DirectX::XMVECTOR surfaceBA = {};
	DirectX::XMVECTOR surfaceCA = {};
	DirectX::XMVECTOR surfaceDotProductVector = {};
	DirectX::XMVECTOR surfaceCentroid = {};
	DirectX::XMVECTOR surfaceNormalNormalized = {};

	FLOAT surfaceDotProduct = 0.0f;

	Point surfacePointA = {};
	Point surfacePointB = {};
	Point surfacePointC = {};

	FLOAT surfaceXCoef = 0.0f;
	FLOAT surfaceYCoef = 0.0f;
	FLOAT surfaceZCoef = 0.0f;
	FLOAT surfaceKCoef = 0.0f;

	BOOL counterClockWise = FALSE;
	BOOL centroidSide = FALSE;

	Point surfaceCentroidPoint = {};
	FLOAT surfaceCentroidScaleCoefficient = 0.5f;

	//Construct clockwise triangles from m_separateVoronoiFacesCulledOrdered without the faces on the unit cube 
	for (UINT i = 0; i < COMPUTE_SHADER_KC_CENTROID_COUNT; ++i) {

		for (UINT j = 0; j < m_pointedVoronoiCells[i].size(); ++j) {

			if (m_pointedVoronoiCells[i][j] < preClipFaceCount && m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]].size() > 0) {

				surfacePointA = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][0];
				surfacePointB = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][1];
				surfacePointC = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][2];

				/*surfacePointA -= 0.5f;
				surfacePointB -= 0.5f;
				surfacePointC -= 0.5f;*/

				//Calculate surface normal
				surfaceBA = DirectX::XMVectorSet(
					surfacePointB.x - surfacePointA.x,
					surfacePointB.y - surfacePointA.y,
					surfacePointB.z - surfacePointA.z,
					1.0f
				);

				surfaceCA = DirectX::XMVectorSet(
					surfacePointC.x - surfacePointA.x,
					surfacePointC.y - surfacePointA.y,
					surfacePointC.z - surfacePointA.z,
					1.0f
				);

				surfaceNormal = DirectX::XMVector3Cross(surfaceBA, surfaceCA);

				////Check the winding order of the points
				//surfaceDotProductVector = DirectX::XMVector3Dot(surfaceNormal, m_cameraPosition);
				//surfaceDotProduct = DirectX::XMVectorGetX(surfaceDotProductVector);

				//if (surfaceDotProduct > 0.0f) {

				//	counterClockWise = TRUE;

				//}
				//else {

				//	counterClockWise = FALSE;

				//}

				//Check the position of the centroid
				surfaceCentroid = DirectX::XMVectorSet(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z, 1.0f);
				
				surfaceXCoef = DirectX::XMVectorGetX(surfaceNormal);
				surfaceYCoef = DirectX::XMVectorGetY(surfaceNormal);
				surfaceZCoef = DirectX::XMVectorGetZ(surfaceNormal);
				surfaceKCoef = -(surfaceXCoef * surfacePointA.x + surfaceYCoef * surfacePointA.y + surfaceZCoef * surfacePointA.z);

				surfaceDotProduct = surfaceXCoef * m_centroids[i].x + surfaceYCoef * m_centroids[i].y + surfaceZCoef * m_centroids[i].z + surfaceKCoef;

				/*surfaceDotProductVector = DirectX::XMVector3Dot(surfaceNormal, surfaceCentroid);
				surfaceDotProduct = DirectX::XMVectorGetX(surfaceDotProductVector);*/

				surfaceNormalNormalized = DirectX::XMVector3Normalize(surfaceNormal);
				surfaceXCoef = DirectX::XMVectorGetX(surfaceNormalNormalized);
				surfaceYCoef = DirectX::XMVectorGetY(surfaceNormalNormalized);
				surfaceZCoef = DirectX::XMVectorGetZ(surfaceNormalNormalized);

				if (surfaceDotProduct > 0.0f) {

					counterClockWise = TRUE;

				}
				else {

					counterClockWise = FALSE;

				}

				surfaceCentroidPoint = Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z);

				//if ((centroidSide == TRUE && counterClockWise == TRUE) || (centroidSide == FALSE && counterClockWise == FALSE)) {
				if (counterClockWise == TRUE) {

					for (INT k = 2; k < m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]].size(); ++k) {

						surfacePointA = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][0];
						surfacePointB = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k - 1];
						surfacePointC = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k];

						surfacePointA -= surfaceCentroidPoint;
						surfacePointB -= surfaceCentroidPoint;
						surfacePointC -= surfaceCentroidPoint;
						
						surfacePointA *= surfaceCentroidScaleCoefficient;
						surfacePointB *= surfaceCentroidScaleCoefficient;
						surfacePointC *= surfaceCentroidScaleCoefficient;

						surfacePointA += surfaceCentroidPoint;
						surfacePointB += surfaceCentroidPoint;
						surfacePointC += surfaceCentroidPoint;

						m_clippedVoronoiCellsBackCulledTriangles.emplace_back(
							m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][0],
							m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k - 1],
							m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k],
							Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z)
						);

						m_clippedVoronoiCellsBackCulledTrianglesNormals.emplace_back(
							surfacePointA,
							surfacePointB,
							surfacePointC,
							Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z),
							Point(
								-surfaceXCoef,
								-surfaceYCoef,
								-surfaceZCoef
							)
						);

					}

				}
				else {

					for (INT k = static_cast<INT>(m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]].size()) - 3; k >= 0; --k) {

						surfacePointA = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]].size() - 1];
						surfacePointB = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k + 1];
						surfacePointC = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k];

						surfacePointA -= surfaceCentroidPoint;
						surfacePointB -= surfaceCentroidPoint;
						surfacePointC -= surfaceCentroidPoint;

						surfacePointA *= surfaceCentroidScaleCoefficient;
						surfacePointB *= surfaceCentroidScaleCoefficient;
						surfacePointC *= surfaceCentroidScaleCoefficient;

						surfacePointA += surfaceCentroidPoint;
						surfacePointB += surfaceCentroidPoint;
						surfacePointC += surfaceCentroidPoint;

						m_clippedVoronoiCellsBackCulledTriangles.emplace_back(
							m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]].size() - 1],
							m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k + 1],
							m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k],
							Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z)
						);

						m_clippedVoronoiCellsBackCulledTrianglesNormals.emplace_back(
							surfacePointA,
							surfacePointB,
							surfacePointC,
							Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z),
							Point(
								surfaceXCoef,
								surfaceYCoef,
								surfaceZCoef
							)
						);

					}

				}

			}

		}

	}

	//Construct clockwise triangles from m_separateVoronoiFacesCulledOrdered for the faces on the unit cube
	for (UINT i = 0; i < COMPUTE_SHADER_KC_CENTROID_COUNT; ++i) {

		for (UINT j = 0; j < m_pointedVoronoiCells[i].size(); ++j) {

			if (m_pointedVoronoiCells[i][j] >= preClipFaceCount && m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]].size() > 0) {

				surfacePointA = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][0];
				surfacePointB = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][1];
				surfacePointC = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][2];

				//Calculate surface normal
				surfaceBA = DirectX::XMVectorSet(
					surfacePointB.x - surfacePointA.x,
					surfacePointB.y - surfacePointA.y,
					surfacePointB.z - surfacePointA.z,
					1.0f
				);

				surfaceCA = DirectX::XMVectorSet(
					surfacePointC.x - surfacePointA.x,
					surfacePointC.y - surfacePointA.y,
					surfacePointC.z - surfacePointA.z,
					1.0f
				);

				surfaceNormal = DirectX::XMVector3Cross(surfaceBA, surfaceCA);

				//Check the position of the centroid
				surfaceCentroid = DirectX::XMVectorSet(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z, 1.0f);

				surfaceXCoef = DirectX::XMVectorGetX(surfaceNormal);
				surfaceYCoef = DirectX::XMVectorGetY(surfaceNormal);
				surfaceZCoef = DirectX::XMVectorGetZ(surfaceNormal);
				surfaceKCoef = -(surfaceXCoef * surfacePointA.x + surfaceYCoef * surfacePointA.y + surfaceZCoef * surfacePointA.z);

				surfaceDotProduct = surfaceXCoef * m_centroids[i].x + surfaceYCoef * m_centroids[i].y + surfaceZCoef * m_centroids[i].z + surfaceKCoef;

				surfaceNormalNormalized = DirectX::XMVector3Normalize(surfaceNormal);
				surfaceXCoef = DirectX::XMVectorGetX(surfaceNormalNormalized);
				surfaceYCoef = DirectX::XMVectorGetY(surfaceNormalNormalized);
				surfaceZCoef = DirectX::XMVectorGetZ(surfaceNormalNormalized);

				if (surfaceDotProduct > 0.0f) {

					counterClockWise = TRUE;

				}
				else {

					counterClockWise = FALSE;

				}

				surfaceCentroidPoint = Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z);

				if (counterClockWise == TRUE) {

					//Normal points toward the centroid
					for (INT k = 2; k < m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]].size(); ++k) {

						surfacePointA = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][0];
						surfacePointB = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k - 1];
						surfacePointC = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k];

						surfacePointA -= surfaceCentroidPoint;
						surfacePointB -= surfaceCentroidPoint;
						surfacePointC -= surfaceCentroidPoint;

						surfacePointA *= surfaceCentroidScaleCoefficient;
						surfacePointB *= surfaceCentroidScaleCoefficient;
						surfacePointC *= surfaceCentroidScaleCoefficient;

						surfacePointA += surfaceCentroidPoint;
						surfacePointB += surfaceCentroidPoint;
						surfacePointC += surfaceCentroidPoint;

						m_clippedVoronoiCellsBackCulledTriangles.emplace_back(
							m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][0],
							m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k - 1],
							m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k],
							Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z)
						);

						if (m_pointedVoronoiCells[i][j] == 5) {

							m_clippedVoronoiCellsBackCulledTrianglesNormals.emplace_back(
								surfacePointA,
								surfacePointB,
								surfacePointC,
								Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z),
								Point(
									-surfaceXCoef,
									-surfaceYCoef,
									-surfaceZCoef
								)
							);

						}
						else {

							m_clippedVoronoiCellsBackCulledTrianglesNormals.emplace_back(
								surfacePointA,
								surfacePointB,
								surfacePointC,
								Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z),
								Point(
									-surfaceXCoef,
									-surfaceYCoef,
									-surfaceZCoef
								)
							);

						}

					}

				}
				else {

					for (INT k = static_cast<INT>(m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]].size()) - 3; k >= 0; --k) {

						surfacePointA = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]].size() - 1];
						surfacePointB = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k + 1];
						surfacePointC = m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k];

						surfacePointA -= surfaceCentroidPoint;
						surfacePointB -= surfaceCentroidPoint;
						surfacePointC -= surfaceCentroidPoint;

						surfacePointA *= surfaceCentroidScaleCoefficient;
						surfacePointB *= surfaceCentroidScaleCoefficient;
						surfacePointC *= surfaceCentroidScaleCoefficient;

						surfacePointA += surfaceCentroidPoint;
						surfacePointB += surfaceCentroidPoint;
						surfacePointC += surfaceCentroidPoint;

						m_clippedVoronoiCellsBackCulledTriangles.emplace_back(
							m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]].size() - 1],
							m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k + 1],
							m_separateVoronoiFacesCulledOrdered[m_pointedVoronoiCells[i][j]][k],
							Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z)
						);

						if (m_pointedVoronoiCells[i][j] == 5) {
						
							m_clippedVoronoiCellsBackCulledTrianglesNormals.emplace_back(
								surfacePointA,
								surfacePointB,
								surfacePointC,
								Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z),
								Point(
									surfaceXCoef,
									surfaceYCoef,
									surfaceZCoef
								)
							);
						
						}
						else {

							m_clippedVoronoiCellsBackCulledTrianglesNormals.emplace_back(
								surfacePointA,
								surfacePointB,
								surfacePointC,
								Point(m_centroids[i].x, m_centroids[i].y, m_centroids[i].z),
								Point(
									surfaceXCoef,
									surfaceYCoef,
									surfaceZCoef
								)
							);

						}

						
					}

				}

			}

		}

	}

	//Construct unit cube edges
	m_unitCubeEdges.emplace_back(Point(0.0f, 0.0f, 0.0f), Point(0.0f, 1.0f, 0.0f));
	m_unitCubeEdges.emplace_back(Point(0.0f, 0.0f, 0.0f), Point(1.0f, 0.0f, 0.0f));
	m_unitCubeEdges.emplace_back(Point(1.0f, 1.0f, 0.0f), Point(0.0f, 1.0f, 0.0f));
	m_unitCubeEdges.emplace_back(Point(1.0f, 1.0f, 0.0f), Point(1.0f, 0.0f, 0.0f));

	m_unitCubeEdges.emplace_back(Point(0.0f, 0.0f, 1.0f), Point(0.0f, 1.0f, 1.0f));
	m_unitCubeEdges.emplace_back(Point(0.0f, 0.0f, 1.0f), Point(1.0f, 0.0f, 1.0f));
	m_unitCubeEdges.emplace_back(Point(1.0f, 1.0f, 1.0f), Point(0.0f, 1.0f, 1.0f));
	m_unitCubeEdges.emplace_back(Point(1.0f, 1.0f, 1.0f), Point(1.0f, 0.0f, 1.0f));

	m_unitCubeEdges.emplace_back(Point(0.0f, 0.0f, 0.0f), Point(0.0f, 0.0f, 1.0f));
	m_unitCubeEdges.emplace_back(Point(0.0f, 1.0f, 0.0f), Point(0.0f, 1.0f, 1.0f));
	m_unitCubeEdges.emplace_back(Point(1.0f, 1.0f, 0.0f), Point(1.0f, 1.0f, 1.0f));
	m_unitCubeEdges.emplace_back(Point(1.0f, 0.0f, 0.0f), Point(1.0f, 0.0f, 1.0f));

	delete[] buffer;

	BOOL isPointA = FALSE;
	BOOL isPointB = FALSE;
	BOOL isPointC = FALSE;
	BOOL isPointD = FALSE;
	for (UINT i = 0; i < m_triangulation.size(); ++i) {

		isPointA = FALSE;
		isPointB = FALSE;
		isPointC = FALSE;
		isPointD = FALSE;
		for (UINT j = 0; j < COMPUTE_SHADER_KC_CENTROID_COUNT; ++j) {

			if (m_triangulation[i].a == m_centroids[j]) isPointA = TRUE;
			if (m_triangulation[i].b == m_centroids[j]) isPointB = TRUE;
			if (m_triangulation[i].c == m_centroids[j]) isPointC = TRUE;
			if (m_triangulation[i].d == m_centroids[j]) isPointD = TRUE;

		}

		if (isPointA && isPointB && isPointC && isPointD) {
		
			m_centroidTetrahedrons.push_back(m_triangulation[i]);

		}

	}


	/*FLOAT downscaleConstant = 1.0f;
	for (UINT i = 0; i < m_voronoiColoredTriangles.size(); ++i) {

		m_voronoiColoredTriangles[i].a *= downscaleConstant;
		m_voronoiColoredTriangles[i].b *= downscaleConstant;
		m_voronoiColoredTriangles[i].c *= downscaleConstant;

	}*/

	/*for (UINT i = 0; i < m_voronoiColoredTriangles.size(); ++i) {

		if (m_voronoiColoredTriangles[i].a.x > 1.0f) m_voronoiColoredTriangles[i].a.x = 1.0f;
		else if (m_voronoiColoredTriangles[i].a.x < 0.0f) m_voronoiColoredTriangles[i].a.x = 0.0f;
		if (m_voronoiColoredTriangles[i].a.y > 1.0f) m_voronoiColoredTriangles[i].a.y = 1.0f;
		else if (m_voronoiColoredTriangles[i].a.y < 0.0f) m_voronoiColoredTriangles[i].a.y = 0.0f;
		if (m_voronoiColoredTriangles[i].a.z > 1.0f) m_voronoiColoredTriangles[i].a.z = 1.0f;
		else if (m_voronoiColoredTriangles[i].a.z < 0.0f) m_voronoiColoredTriangles[i].a.z = 0.0f;
		
		if (m_voronoiColoredTriangles[i].b.x > 1.0f) m_voronoiColoredTriangles[i].b.x = 1.0f;
		else if (m_voronoiColoredTriangles[i].b.x < 0.0f) m_voronoiColoredTriangles[i].b.x = 0.0f;
		if (m_voronoiColoredTriangles[i].b.y > 1.0f) m_voronoiColoredTriangles[i].b.y = 1.0f;
		else if (m_voronoiColoredTriangles[i].b.y < 0.0f) m_voronoiColoredTriangles[i].b.y = 0.0f;
		if (m_voronoiColoredTriangles[i].b.z > 1.0f) m_voronoiColoredTriangles[i].b.z = 1.0f;
		else if (m_voronoiColoredTriangles[i].b.z < 0.0f) m_voronoiColoredTriangles[i].b.z = 0.0f;

		if (m_voronoiColoredTriangles[i].c.x > 1.0f) m_voronoiColoredTriangles[i].c.x = 1.0f;
		else if (m_voronoiColoredTriangles[i].c.x < 0.0f) m_voronoiColoredTriangles[i].c.x = 0.0f;
		if (m_voronoiColoredTriangles[i].c.y > 1.0f) m_voronoiColoredTriangles[i].c.y = 1.0f;
		else if (m_voronoiColoredTriangles[i].c.y < 0.0f) m_voronoiColoredTriangles[i].c.y = 0.0f;
		if (m_voronoiColoredTriangles[i].c.z > 1.0f) m_voronoiColoredTriangles[i].c.z = 1.0f;
		else if (m_voronoiColoredTriangles[i].c.z < 0.0f) m_voronoiColoredTriangles[i].c.z = 0.0f;

	}*/

	//DirectX render pipeline setup

	this->LoadShaders();

	this->InitVertexBufferTriangleList();
	this->InitVertexBufferTriangleListColor();
	this->InitVertexBufferTriangleListTetrahedronColor();
	//this->InitVertexBufferTriangleListClippedVoronoiFaces();
	this->InitVertexBufferTriangleListClippedVoronoiCells();
	this->InitVertexBufferTriangleListClippedVoronoiCellsBackCull();
	this->InitVertexBufferTriangleListClippedVoronoiCellsBackCullNormals();
	this->InitVertexBufferTriangleListClippedVoronoiFacesNormals();
	this->InitVertexBufferLineList();
	this->InitVertexBufferLineListDelaunayEdgesColor();
	this->InitVertexBufferLineListNoColor();
	this->InitVertexBufferLineListVoronoiFacesNoColor();
	this->InitVertexBufferLineListUnitCubeNoColor();
	this->InitVertexBufferLineListVoronoiEdgesUnitCubeColor();
	this->InitDepthBuffer();
	this->InitRootSignature();
	this->InitRootSignatureLighting();
	this->InitPipelineStateObjectTriangleList();
	this->InitPipelineStateObjectTriangleListColor();
	this->InitPipelineStateObjectTriangleListColorBackCull();
	this->InitPipelineStateObjectTriangleListColorBackCullNormal();
	this->InitPipelineStateObjectTriangleListColorNormal();
	this->InitPipelineStateObjectLineList();
	this->InitPipelineStateObjectLineListNoColor();

	*updateFPS = 0.0001f;
	m_isLoaded = TRUE;

	return;

}

BOOL SubspaceRender::OnRender(mWRL::ComPtr<ID3D12GraphicsCommandList> commandList, mWRL::ComPtr<ID3D12Resource> backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE backBufferDescriptor) {

	if (!m_isLoaded) {

		return FALSE;

	}

	//Get depth buffer descriptor
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptor = m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	//Set back buffer state to D3D12_RESOURCE_STATE_RENDER_TARGET
	{
		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = backBuffer.Get();
		resourceBarrier.Transition.Subresource = 0;
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		commandList->ResourceBarrier(1, &resourceBarrier);
	}

	//Clear the back buffer and the depth-stencil buffer
	FLOAT backBufferClearValue[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D12_RECT clearRectangle = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	commandList->ClearRenderTargetView(backBufferDescriptor, backBufferClearValue, 1, &clearRectangle);				//Back buffer clear
	commandList->ClearDepthStencilView(depthStencilDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);			//Depth stencil buffer clear
	
	//First pass - line list - delaunay edges
	{

		//Set pipeline state object and root signature
		commandList->SetPipelineState(m_pipelineStateLineList->GetPipelineStateObject().Get());
		commandList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature().Get());

		//Set primitive topology
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		//Set vertex buffer
		commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewLineListDelaunayEdgesColor);

		//Set root signature parameters
		commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_worldViewProjectionMatrix, 0);

		//Set scissor rectangle and viewport
		commandList->RSSetViewports(1, &m_viewport);
		commandList->RSSetScissorRects(1, &m_scissorRectangle);

		//Set render target
		commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

		//Draw
		commandList->DrawInstanced(static_cast<UINT>(m_delaunayEdges.size()) * 2, 1, 0, 0);

	}

	//Second pass - line list - unit cube no color
	{

		////Set pipeline state object and root signature
		//commandList->SetPipelineState(m_pipelineStateLineListNoColor->GetPipelineStateObject().Get());
		//commandList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature().Get());

		////Set primitive topology
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		////Set vertex buffer
		//commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewLineListUnitCubeNoColor);

		////Set root signature parameters
		//commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_worldViewProjectionMatrix, 0);

		////Set scissor rectangle and viewport
		//commandList->RSSetViewports(1, &m_viewport);
		//commandList->RSSetScissorRects(1, &m_scissorRectangle);

		////Set render target
		//commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

		////Draw
		//commandList->DrawInstanced(static_cast<UINT>(m_unitCubeEdges.size()) * 2, 1, 0, 0);

	}

	//Second pass - line list - voronoi edges including those that lie on the unit cube
	{

		//Set pipeline state object and root signature
		commandList->SetPipelineState(m_pipelineStateLineList->GetPipelineStateObject().Get());
		commandList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature().Get());

		//Set primitive topology
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		//Set vertex buffer
		commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewLineListVoronoiEdgesUnitCubeColor);

		//Set root signature parameters
		commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_worldViewProjectionMatrix, 0);

		//Set scissor rectangle and viewport
		commandList->RSSetViewports(1, &m_viewport);
		commandList->RSSetScissorRects(1, &m_scissorRectangle);

		//Set render target
		commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

		//Draw
		commandList->DrawInstanced(static_cast<UINT>(m_voronoiEdgesUnitCube.size()) * 2, 1, 0, 0);

	}

	//Second pass - triangle list
	{

		////Set pipeline state object and root signature
		//commandList->SetPipelineState(m_pipelineStateTriangleList->GetPipelineStateObject().Get());
		//commandList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature().Get());

		////Set primitive topology
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		////Set vertex buffer
		//commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewTriangleList);

		////Set root signature parameters
		//commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_viewProjectionMatrix, 0);

		////Set scissor rectangle and viewport
		//commandList->RSSetViewports(1, &m_viewport);
		//commandList->RSSetScissorRects(1, &m_scissorRectangle);

		////Set render target
		//commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

		////Draw
		//commandList->DrawInstanced(static_cast<UINT>(m_triangulation.size()) * 4 * 3, 1, 0, 0);
	
	}

	//Second pass - line list
	{

		////Set pipeline state object and root signature
		//commandList->SetPipelineState(m_pipelineStateLineList->GetPipelineStateObject().Get());
		//commandList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature().Get());

		////Set primitive topology
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		////Set vertex buffer
		//commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewLineList);

		////Set root signature parameters
		//commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_viewProjectionMatrix, 0);

		////Set scissor rectangle and viewport
		//commandList->RSSetViewports(1, &m_viewport);
		//commandList->RSSetScissorRects(1, &m_scissorRectangle);

		////Set render target
		//commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

		////Draw
		//commandList->DrawInstanced(static_cast<UINT>(m_voronoiColoredEdges.size()) * 2, 1, 0, 0);

	}

	if (m_unitCubeClipEnabled == 0) {
	
		//Third pass - colored triangle list
		{

			//Set pipeline state object and root signature
			commandList->SetPipelineState(m_pipelineStateTriangleListColor->GetPipelineStateObject().Get());
			commandList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature().Get());

			//Set primitive topology
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//Set vertex buffer
			commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewTriangleListColor);

			//Set root signature parameters
			commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_worldViewProjectionMatrix, 0);

			//Set scissor rectangle and viewport
			commandList->RSSetViewports(1, &m_viewport);
			commandList->RSSetScissorRects(1, &m_scissorRectangle);

			//Set render target
			commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

			//Draw
			commandList->DrawInstanced(static_cast<UINT>(m_voronoiColoredTriangles.size()) * 3, 1, 0, 0);

		}
	
	}
	else if (m_unitCubeClipEnabled == 1) {

		//Third pass - colored triangle list
		{

			////Set pipeline state object and root signature
			//commandList->SetPipelineState(m_pipelineStateTriangleListColor->GetPipelineStateObject().Get());
			//commandList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature().Get());

			////Set primitive topology
			//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			////Set vertex buffer
			//commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewTriangleListClippedVoronoiFaces);

			////Set root signature parameters
			//commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_worldViewProjectionMatrix, 0);

			////Set scissor rectangle and viewport
			//commandList->RSSetViewports(1, &m_viewport);
			//commandList->RSSetScissorRects(1, &m_scissorRectangle);

			////Set render target
			//commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

			////Draw
			//commandList->DrawInstanced(static_cast<UINT>(m_clippedVoronoiTriangles.size()) * 3, 1, 0, 0);

		}

		//Third pass - colored triangle list
		{

			////Set pipeline state object and root signature
			//commandList->SetPipelineState(m_pipelineStateTriangleListColorBackCull->GetPipelineStateObject().Get());
			//commandList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature().Get());

			////Set primitive topology
			//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			////Set vertex buffer
			//commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewTriangleListClippedVoronoiCellsBackCull);

			////Set root signature parameters
			//commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_worldViewProjectionMatrix, 0);

			////Set scissor rectangle and viewport
			//commandList->RSSetViewports(1, &m_viewport);
			//commandList->RSSetScissorRects(1, &m_scissorRectangle);

			////Set blending
			//FLOAT blendFactor[4] = {0.5f, 0.5f, 0.5f, 1.0f};
			//commandList->OMSetBlendFactor(blendFactor);

			////Set render target
			//commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

			////Draw
			//commandList->DrawInstanced(static_cast<UINT>(m_clippedVoronoiCellsBackCulledTriangles.size()) * 3, 1, 0, 0);

		}

		//Third pass - colored triangle list
		{

			//Set pipeline state object and root signature
			commandList->SetPipelineState(m_pipelineStateTriangleListColorBackCullNormal->GetPipelineStateObject().Get());
			commandList->SetGraphicsRootSignature(m_rootSignatureLighting->GetRootSignature().Get());

			//Set primitive topology
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//Set vertex buffer
			commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewTriangleListClippedVoronoiCellsBackCullNormals);

			//Set root signature parameters
			commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_worldMatrix, 0);
			commandList->SetGraphicsRoot32BitConstants(1, sizeof(DirectX::XMMATRIX) / 4, &m_viewMatrix, 0);
			commandList->SetGraphicsRoot32BitConstants(2, sizeof(DirectX::XMMATRIX) / 4, &m_projectionMatrix, 0);
			commandList->SetGraphicsRoot32BitConstants(3, sizeof(DirectX::XMVECTOR) / 4, &m_cameraPosition, 0);

			//Set scissor rectangle and viewport
			commandList->RSSetViewports(1, &m_viewport);
			commandList->RSSetScissorRects(1, &m_scissorRectangle);

			//Set blending
			//FLOAT blendFactor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
			FLOAT blendFactor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			commandList->OMSetBlendFactor(blendFactor);

			//Set render target
			commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

			//Draw
			commandList->DrawInstanced(static_cast<UINT>(m_clippedVoronoiCellsBackCulledTrianglesNormals.size()) * 3, 1, 0, 0);

		}

	}
	else if (m_unitCubeClipEnabled == 2) {

		//Third pass - colored triangle list
		{

			//Set pipeline state object and root signature
			commandList->SetPipelineState(m_pipelineStateTriangleListColor->GetPipelineStateObject().Get());
			commandList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature().Get());

			//Set primitive topology
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//Set vertex buffer
			commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewTriangleListClippedVoronoiCells);

			//Set root signature parameters
			commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_worldViewProjectionMatrix, 0);

			//Set scissor rectangle and viewport
			commandList->RSSetViewports(1, &m_viewport);
			commandList->RSSetScissorRects(1, &m_scissorRectangle);

			//Set render target
			commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

			//Draw
			commandList->DrawInstanced(static_cast<UINT>(m_clippedVoronoiCellsTriangles.size()) * 3, 1, 0, 0);
		
		}

	}
	else if (m_unitCubeClipEnabled == 3) {

		//Fourth pass - colored triangle list with normals included
		{

			//Set pipeline state object and root signature
			commandList->SetPipelineState(m_pipelineStateTriangleListColorNormal->GetPipelineStateObject().Get());
			commandList->SetGraphicsRootSignature(m_rootSignatureLighting->GetRootSignature().Get());

			//Set primitive topology
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//Set vertex buffer
			commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewTriangleListClippedVoronoiFacesNormals);

			//Set root signature parameters
			commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_worldMatrix, 0);
			commandList->SetGraphicsRoot32BitConstants(1, sizeof(DirectX::XMMATRIX) / 4, &m_viewMatrix, 0);
			commandList->SetGraphicsRoot32BitConstants(2, sizeof(DirectX::XMMATRIX) / 4, &m_projectionMatrix, 0);
			commandList->SetGraphicsRoot32BitConstants(3, sizeof(DirectX::XMVECTOR) / 4, &m_cameraPosition, 0);

			//Set scissor rectangle and viewport
			commandList->RSSetViewports(1, &m_viewport);
			commandList->RSSetScissorRects(1, &m_scissorRectangle);

			//Set render target
			commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

			//Draw
			commandList->DrawInstanced(static_cast<UINT>(m_clippedVoronoiTrianglesFaceNormals.size()) * 3, 1, 0, 0);

		}

	}

	//Fourth pass - colored triangle list of tetrahedrons in m_triangulation
	{
	
		////Set pipeline state object and root signature
		//commandList->SetPipelineState(m_pipelineStateTriangleListColor->GetPipelineStateObject().Get());
		//commandList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature().Get());

		////Set primitive topology
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		////Set vertex buffer
		//commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewTriangleListTetrahedronColor);

		////Set root signature parameters
		//commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_viewProjectionMatrix, 0);

		////Set scissor rectangle and viewport
		//commandList->RSSetViewports(1, &m_viewport);
		//commandList->RSSetScissorRects(1, &m_scissorRectangle);

		////Set render target
		//commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

		////Draw
		//commandList->DrawInstanced(12, 1, 12 * m_tetrahedronVisualizationIndex, 0);
	
	}

	//Fourth pass - line list - voronoi faces (voronoi edges) one by one
	{

		////Set pipeline state object and root signature
		//commandList->SetPipelineState(m_pipelineStateLineListNoColor->GetPipelineStateObject().Get());
		//commandList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature().Get());

		////Set primitive topology
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		////Set vertex buffer
		//commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewLineListVoronoiFacesNoColor);

		////Set root signature parameters
		//commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_viewProjectionMatrix, 0);

		////Set scissor rectangle and viewport
		//commandList->RSSetViewports(1, &m_viewport);
		//commandList->RSSetScissorRects(1, &m_scissorRectangle);

		////Set render target
		//commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

		////Draw
		//commandList->DrawInstanced(m_voronoiFacesCount[m_voronoiFaceVisualizationIndex] * 2, 1, m_voronoiFaceVisualizationOffset * 2, 0);

	}

	//Fourth pass - line list no color - all voronoi edges
	{
	
		////Set pipeline state object and root signature
		//commandList->SetPipelineState(m_pipelineStateLineListNoColor->GetPipelineStateObject().Get());
		//commandList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature().Get());

		////Set primitive topology
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		////Set vertex buffer
		//commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewLineListNoColor);

		////Set root signature parameters
		//commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_viewProjectionMatrix, 0);

		////Set scissor rectangle and viewport
		//commandList->RSSetViewports(1, &m_viewport);
		//commandList->RSSetScissorRects(1, &m_scissorRectangle);

		////Set render target
		//commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

		////Draw
		//commandList->DrawInstanced(static_cast<UINT>(m_voronoiEdges.size()) * 2, 1, 0, 0);
	
	}

	//Set back buffer state to D3D12_RESOURCE_STATE_PRESENT
	{
		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = backBuffer.Get();
		resourceBarrier.Transition.Subresource = 0;
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		commandList->ResourceBarrier(1, &resourceBarrier);
	}

	return TRUE;

}

BOOL SubspaceRender::OnUpdate(double elapsedTime, BYTE flag) {

	if (!m_isLoaded) {

		return FALSE;

	}

	return TRUE;

}

void SubspaceRender::OnMouseWheel(float mouseWheelData) {

	DirectX::XMMATRIX scalingMatrix = {};
	if (mouseWheelData > 0.0f) {

		scalingMatrix = DirectX::XMMatrixScaling(0.9f, 0.9f, 0.9f);

	}
	else {

		scalingMatrix = DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f);

	}

	m_cameraPosition = DirectX::XMVector4Transform(m_cameraPosition, scalingMatrix);
	const DirectX::XMVECTOR focusPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_viewMatrix = DirectX::XMMatrixLookAtLH(m_cameraPosition, focusPosition, upDirection);

	//Update the world view projection matrix
	m_worldViewProjectionMatrix = DirectX::XMMatrixMultiply(m_worldMatrix, m_viewMatrix);
	m_worldViewProjectionMatrix = DirectX::XMMatrixMultiply(m_worldViewProjectionMatrix, m_projectionMatrix);
	
}

void SubspaceRender::OnMouseButtonLeftMove(WORD mX1, WORD mX2, WORD mY1, WORD mY2) {

	//Get unit vectors
	DirectX::XMVECTOR v1 = this->GetUnitVector(mX1, mY1);
	DirectX::XMVECTOR v2 = this->GetUnitVector(mX2, mY2);

	//Get rotation angle
	DirectX::XMVECTOR rotationAngleVector = DirectX::XMVector3Dot(v1, v2);
	FLOAT rotationAngle = DirectX::XMVectorGetX(rotationAngleVector);
	rotationAngle = min(1.0f, acosf(rotationAngle)) * 2.0f;

	//Get rotation axis
	DirectX::XMVECTOR rotationAxis = DirectX::XMVector3Cross(v1, v2);

	//Set view matrix
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, rotationAngle);

	m_worldMatrix = DirectX::XMMatrixMultiply(m_worldMatrix, rotationMatrix);

	//Update the world view projection matrix
	m_worldViewProjectionMatrix = DirectX::XMMatrixMultiply(m_worldMatrix, m_viewMatrix);
	m_worldViewProjectionMatrix = DirectX::XMMatrixMultiply(m_worldViewProjectionMatrix, m_projectionMatrix);

}

void SubspaceRender::OnSpace() {

	/*++m_tetrahedronVisualizationIndex;
	m_voronoiFaceVisualizationOffset += m_voronoiFacesCount[m_voronoiFaceVisualizationIndex];
	++m_voronoiFaceVisualizationIndex;*/

	++m_unitCubeClipEnabled;
	m_unitCubeClipEnabled = m_unitCubeClipEnabled % 4;

}

SubspaceRender::SubspaceRender(Microsoft::WRL::ComPtr<ID3D12Device2> dxDevice, std::shared_ptr<CommandQueue> copyCQ, std::shared_ptr<CommandQueue> directCQ)
	: m_dxDevice(dxDevice), m_copyCQ(copyCQ), m_directCQ(directCQ) {

	m_vertexBufferViewTriangleList = { 0 };
	m_vertexBufferViewTriangleListColor = { 0 };
	m_vertexBufferViewTriangleListTetrahedronColor = { 0 };
	m_vertexBufferViewTriangleListClippedVoronoiFaces = { 0 };
	m_vertexBufferViewTriangleListClippedVoronoiCells = { 0 };
	m_vertexBufferViewTriangleListClippedVoronoiCellsBackCull = { 0 };
	m_vertexBufferViewTriangleListClippedVoronoiCellsBackCullNormals = { 0 };
	m_vertexBufferViewTriangleListClippedVoronoiFacesNormals = { 0 };
	m_vertexBufferViewLineList = { 0 };
	m_vertexBufferViewLineListDelaunayEdgesColor = { 0 };
	m_vertexBufferViewLineListNoColor = { 0 };
	m_vertexBufferViewLineListVoronoiFacesNoColor = { 0 };
	m_vertexBufferViewLineListUnitCubeNoColor = { 0 };
	m_vertexBufferViewLineListVoronoiEdgesUnitCubeColor = { 0 };
	std::srand(static_cast<UINT>(std::time(nullptr)));

	//Check root signature version support
	m_rootSignatureVersionSupport.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(m_dxDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &m_rootSignatureVersionSupport, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE)))) {
		m_rootSignatureVersionSupport.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	//Create world, view and projection matrices
	
	m_worldMatrix = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	m_cameraPosition = DirectX::XMVectorSet(0.0f, 0.0f, -4.0f, 1.0f);
	const DirectX::XMVECTOR focusPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_viewMatrix = DirectX::XMMatrixLookAtLH(m_cameraPosition, focusPosition, upDirection);

	const float aspectRatio = ((float)WINDOW_WIDTH) / ((float)WINDOW_HEIGHT);
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90.0f), aspectRatio, 0.1f, 100.0f);

	m_worldViewProjectionMatrix = DirectX::XMMatrixMultiply(m_worldMatrix, m_viewMatrix);
	m_worldViewProjectionMatrix = DirectX::XMMatrixMultiply(m_worldViewProjectionMatrix, m_projectionMatrix);
	
	//Set viewport and scissor rectangle
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = static_cast<FLOAT>(WINDOW_WIDTH);
	m_viewport.Height = static_cast<FLOAT>(WINDOW_HEIGHT);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	m_scissorRectangle.left = 0;
	m_scissorRectangle.top = 0;
	m_scissorRectangle.right = LONG_MAX;
	m_scissorRectangle.bottom = LONG_MAX;

}

SubspaceRender::~SubspaceRender() {}



//Calculate the circumsphere of the i-th tetrahedron in m_triangulation
void SubspaceRender::CalculateCircumsphere(UINT i) {

	DirectX::XMMATRIX matrixA = {};
	DirectX::XMMATRIX matrixC = {};
	DirectX::XMMATRIX matrixDx = {};
	DirectX::XMMATRIX matrixDy = {};
	DirectX::XMMATRIX matrixDz = {};

	DirectX::XMVECTOR vectorA = {};
	DirectX::XMVECTOR vectorC = {};
	DirectX::XMVECTOR vectorDx = {};
	DirectX::XMVECTOR vectorDy = {};
	DirectX::XMVECTOR vectorDz = {};

	FLOAT determinantA = 0.0f;
	FLOAT determinantC = 0.0f;
	FLOAT determinantDx = 0.0f;
	FLOAT determinantDy = 0.0f;
	FLOAT determinantDz = 0.0f;

	FLOAT pointASq = 0.0f;
	FLOAT pointBSq = 0.0f;
	FLOAT pointCSq = 0.0f;
	FLOAT pointDSq = 0.0f;

	FLOAT determinantDSq = 0.0f;

	pointASq = m_triangulation[i].a.x * m_triangulation[i].a.x + m_triangulation[i].a.y * m_triangulation[i].a.y + m_triangulation[i].a.z * m_triangulation[i].a.z;
	pointBSq = m_triangulation[i].b.x * m_triangulation[i].b.x + m_triangulation[i].b.y * m_triangulation[i].b.y + m_triangulation[i].b.z * m_triangulation[i].b.z;
	pointCSq = m_triangulation[i].c.x * m_triangulation[i].c.x + m_triangulation[i].c.y * m_triangulation[i].c.y + m_triangulation[i].c.z * m_triangulation[i].c.z;
	pointDSq = m_triangulation[i].d.x * m_triangulation[i].d.x + m_triangulation[i].d.y * m_triangulation[i].d.y + m_triangulation[i].d.z * m_triangulation[i].d.z;

	matrixA = DirectX::XMMATRIX(

		m_triangulation[i].a.x, m_triangulation[i].a.y, m_triangulation[i].a.z, 1.0f,
		m_triangulation[i].b.x, m_triangulation[i].b.y, m_triangulation[i].b.z, 1.0f,
		m_triangulation[i].c.x, m_triangulation[i].c.y, m_triangulation[i].c.z, 1.0f,
		m_triangulation[i].d.x, m_triangulation[i].d.y, m_triangulation[i].d.z, 1.0f

	);

	matrixC = DirectX::XMMATRIX(

		pointASq, m_triangulation[i].a.x, m_triangulation[i].a.y, m_triangulation[i].a.z,
		pointBSq, m_triangulation[i].b.x, m_triangulation[i].b.y, m_triangulation[i].b.z,
		pointCSq, m_triangulation[i].c.x, m_triangulation[i].c.y, m_triangulation[i].c.z,
		pointDSq, m_triangulation[i].d.x, m_triangulation[i].d.y, m_triangulation[i].d.z

	);

	matrixDx = DirectX::XMMATRIX(

		pointASq, m_triangulation[i].a.y, m_triangulation[i].a.z, 1.0f,
		pointBSq, m_triangulation[i].b.y, m_triangulation[i].b.z, 1.0f,
		pointCSq, m_triangulation[i].c.y, m_triangulation[i].c.z, 1.0f,
		pointDSq, m_triangulation[i].d.y, m_triangulation[i].d.z, 1.0f

	);

	matrixDy = DirectX::XMMATRIX(

		pointASq, m_triangulation[i].a.x, m_triangulation[i].a.z, 1.0f,
		pointBSq, m_triangulation[i].b.x, m_triangulation[i].b.z, 1.0f,
		pointCSq, m_triangulation[i].c.x, m_triangulation[i].c.z, 1.0f,
		pointDSq, m_triangulation[i].d.x, m_triangulation[i].d.z, 1.0f

	);

	matrixDz = DirectX::XMMATRIX(

		pointASq, m_triangulation[i].a.x, m_triangulation[i].a.y, 1.0f,
		pointBSq, m_triangulation[i].b.x, m_triangulation[i].b.y, 1.0f,
		pointCSq, m_triangulation[i].c.x, m_triangulation[i].c.y, 1.0f,
		pointDSq, m_triangulation[i].d.x, m_triangulation[i].d.y, 1.0f

	);

	vectorA = DirectX::XMMatrixDeterminant(matrixA);
	vectorC = DirectX::XMMatrixDeterminant(matrixC);
	vectorDx = DirectX::XMMatrixDeterminant(matrixDx);
	vectorDy = DirectX::XMMatrixDeterminant(matrixDy);
	vectorDz = DirectX::XMMatrixDeterminant(matrixDz);

	determinantA = DirectX::XMVectorGetX(vectorA);
	determinantC = DirectX::XMVectorGetX(vectorC);
	determinantDx = DirectX::XMVectorGetX(vectorDx);
	determinantDy = -DirectX::XMVectorGetX(vectorDy);
	determinantDz = DirectX::XMVectorGetX(vectorDz);

	determinantDSq = determinantDx * determinantDx + determinantDy * determinantDy + determinantDz * determinantDz;

	m_triangulation[i].circumcenter = Point(determinantDx / (2.0f * determinantA), determinantDy / (2.0f * determinantA), determinantDz / (2.0f * determinantA));
	m_triangulation[i].circumradius = sqrtf(determinantDSq - 4.0f * determinantA * determinantC) / (2.0f * abs(determinantA));

}

//Calculate the circumsphere of the i-th tetrahedron in m_triangulation
void SubspaceRender::CalculateCircumsphere(Tetrahedron* tetrahedron) {

	DirectX::XMMATRIX matrixA = {};
	DirectX::XMMATRIX matrixC = {};
	DirectX::XMMATRIX matrixDx = {};
	DirectX::XMMATRIX matrixDy = {};
	DirectX::XMMATRIX matrixDz = {};

	DirectX::XMVECTOR vectorA = {};
	DirectX::XMVECTOR vectorC = {};
	DirectX::XMVECTOR vectorDx = {};
	DirectX::XMVECTOR vectorDy = {};
	DirectX::XMVECTOR vectorDz = {};

	FLOAT determinantA = 0.0f;
	FLOAT determinantC = 0.0f;
	FLOAT determinantDx = 0.0f;
	FLOAT determinantDy = 0.0f;
	FLOAT determinantDz = 0.0f;

	FLOAT pointASq = 0.0f;
	FLOAT pointBSq = 0.0f;
	FLOAT pointCSq = 0.0f;
	FLOAT pointDSq = 0.0f;

	FLOAT determinantDSq = 0.0f;

	pointASq = (*tetrahedron).a.x * (*tetrahedron).a.x + (*tetrahedron).a.y * (*tetrahedron).a.y + (*tetrahedron).a.z * (*tetrahedron).a.z;
	pointBSq = (*tetrahedron).b.x * (*tetrahedron).b.x + (*tetrahedron).b.y * (*tetrahedron).b.y + (*tetrahedron).b.z * (*tetrahedron).b.z;
	pointCSq = (*tetrahedron).c.x * (*tetrahedron).c.x + (*tetrahedron).c.y * (*tetrahedron).c.y + (*tetrahedron).c.z * (*tetrahedron).c.z;
	pointDSq = (*tetrahedron).d.x * (*tetrahedron).d.x + (*tetrahedron).d.y * (*tetrahedron).d.y + (*tetrahedron).d.z * (*tetrahedron).d.z;

	matrixA = DirectX::XMMATRIX(

		(*tetrahedron).a.x, (*tetrahedron).a.y, (*tetrahedron).a.z, 1.0f,
		(*tetrahedron).b.x, (*tetrahedron).b.y, (*tetrahedron).b.z, 1.0f,
		(*tetrahedron).c.x, (*tetrahedron).c.y, (*tetrahedron).c.z, 1.0f,
		(*tetrahedron).d.x, (*tetrahedron).d.y, (*tetrahedron).d.z, 1.0f

	);

	matrixC = DirectX::XMMATRIX(

		pointASq, (*tetrahedron).a.x, (*tetrahedron).a.y, (*tetrahedron).a.z,
		pointBSq, (*tetrahedron).b.x, (*tetrahedron).b.y, (*tetrahedron).b.z,
		pointCSq, (*tetrahedron).c.x, (*tetrahedron).c.y, (*tetrahedron).c.z,
		pointDSq, (*tetrahedron).d.x, (*tetrahedron).d.y, (*tetrahedron).d.z

	);

	matrixDx = DirectX::XMMATRIX(

		pointASq, (*tetrahedron).a.y, (*tetrahedron).a.z, 1.0f,
		pointBSq, (*tetrahedron).b.y, (*tetrahedron).b.z, 1.0f,
		pointCSq, (*tetrahedron).c.y, (*tetrahedron).c.z, 1.0f,
		pointDSq, (*tetrahedron).d.y, (*tetrahedron).d.z, 1.0f

	);

	matrixDy = DirectX::XMMATRIX(

		pointASq, (*tetrahedron).a.x, (*tetrahedron).a.z, 1.0f,
		pointBSq, (*tetrahedron).b.x, (*tetrahedron).b.z, 1.0f,
		pointCSq, (*tetrahedron).c.x, (*tetrahedron).c.z, 1.0f,
		pointDSq, (*tetrahedron).d.x, (*tetrahedron).d.z, 1.0f

	);

	matrixDz = DirectX::XMMATRIX(

		pointASq, (*tetrahedron).a.x, (*tetrahedron).a.y, 1.0f,
		pointBSq, (*tetrahedron).b.x, (*tetrahedron).b.y, 1.0f,
		pointCSq, (*tetrahedron).c.x, (*tetrahedron).c.y, 1.0f,
		pointDSq, (*tetrahedron).d.x, (*tetrahedron).d.y, 1.0f

	);

	vectorA = DirectX::XMMatrixDeterminant(matrixA);
	vectorC = DirectX::XMMatrixDeterminant(matrixC);
	vectorDx = DirectX::XMMatrixDeterminant(matrixDx);
	vectorDy = DirectX::XMMatrixDeterminant(matrixDy);
	vectorDz = DirectX::XMMatrixDeterminant(matrixDz);

	determinantA = DirectX::XMVectorGetX(vectorA);
	determinantC = DirectX::XMVectorGetX(vectorC);
	determinantDx = DirectX::XMVectorGetX(vectorDx);
	determinantDy = -DirectX::XMVectorGetX(vectorDy);
	determinantDz = DirectX::XMVectorGetX(vectorDz);

	determinantDSq = determinantDx * determinantDx + determinantDy * determinantDy + determinantDz * determinantDz;

	(*tetrahedron).circumcenter = Point(determinantDx / (2.0f * determinantA), determinantDy / (2.0f * determinantA), determinantDz / (2.0f * determinantA));
	(*tetrahedron).circumradius = sqrtf(determinantDSq - 4.0f * determinantA * determinantC) / (2.0f * abs(determinantA));

}

//Check if a triangle is a face of a tetrahedron
BOOL SubspaceRender::IsFace(Point* a, Point* b, Point* c, Tetrahedron* tetrahedron) {

	Triangle faceToCheck = Triangle(Point(*a), Point(*b), Point(*c));

	Triangle tetrahedronFace = {};
	tetrahedronFace.a = tetrahedron->a; tetrahedronFace.b = tetrahedron->b; tetrahedronFace.c = tetrahedron->c;
	if (faceToCheck == tetrahedronFace) return TRUE;
	tetrahedronFace.a = tetrahedron->a; tetrahedronFace.b = tetrahedron->b; tetrahedronFace.c = tetrahedron->d;
	if (faceToCheck == tetrahedronFace) return TRUE;
	tetrahedronFace.a = tetrahedron->a; tetrahedronFace.b = tetrahedron->c; tetrahedronFace.c = tetrahedron->d;
	if (faceToCheck == tetrahedronFace) return TRUE;
	tetrahedronFace.a = tetrahedron->b; tetrahedronFace.b = tetrahedron->c; tetrahedronFace.c = tetrahedron->d;
	if (faceToCheck == tetrahedronFace) return TRUE;

	return FALSE;

}

//Check if a line is an edge of a tetrahedron
BOOL SubspaceRender::IsEdge(Point* a, Point* b, Tetrahedron* tetrahedron) {

	Edge edgeToCheck = Edge(Point(*a), Point(*b));

	Edge tetrahedronEdge = {};
	tetrahedronEdge.a = tetrahedron->a; tetrahedronEdge.b = tetrahedron->b;
	if (edgeToCheck == tetrahedronEdge) return TRUE;
	tetrahedronEdge.a = tetrahedron->a; tetrahedronEdge.b = tetrahedron->c;
	if (edgeToCheck == tetrahedronEdge) return TRUE;
	tetrahedronEdge.a = tetrahedron->a; tetrahedronEdge.b = tetrahedron->d;
	if (edgeToCheck == tetrahedronEdge) return TRUE;
	tetrahedronEdge.a = tetrahedron->b; tetrahedronEdge.b = tetrahedron->c;
	if (edgeToCheck == tetrahedronEdge) return TRUE;
	tetrahedronEdge.a = tetrahedron->b; tetrahedronEdge.b = tetrahedron->d;
	if (edgeToCheck == tetrahedronEdge) return TRUE;
	tetrahedronEdge.a = tetrahedron->c; tetrahedronEdge.b = tetrahedron->d;
	if (edgeToCheck == tetrahedronEdge) return TRUE;

	
	return FALSE;

}

//Returns true if line intersects plane
BOOL SubspaceRender::LinePlaneIntersection(Point* lineA, Point* lineB, FLOAT planeXCoef, FLOAT planeYCoef, FLOAT planeZCoef, FLOAT planeKCoef, Point* intersection) {

	//Parametric form of line equation
	//x = x0 + dx * t
	//y = y0 + dy * t
	//z = z0 + dz * t
	//
	//Where (dx, dy, dz) is the direction vector
	
	//Get direction vector: dx = x0 - x1, dy = y0 - y1, dz = z0 - z1
	FLOAT directionX = lineA->x - lineB->x;
	FLOAT directionY = lineA->y - lineB->y;
	FLOAT directionZ = lineA->z - lineB->z;

	//Check if line is not parallel to the plane or is on the plane (particular case of being parallel to the plane)
	DirectX::XMVECTOR planeNormalVector = DirectX::XMVectorSet(planeXCoef, planeYCoef, planeZCoef, 1.0f);
	DirectX::XMVECTOR directionVector = DirectX::XMVectorSet(directionX, directionY, directionZ, 1.0f);
	DirectX::XMVECTOR dotProductVector = DirectX::XMVector3Dot(planeNormalVector, directionVector);
	FLOAT dotProduct = DirectX::XMVectorGetX(dotProductVector);

	if (dotProduct == 0) {

		return FALSE;

	}

	//Plane equation: planeXCoef * x + planeYCoef * y + planeZCoef * z + planeKCoef = 0
	//Substitute parametric form of line into plane equation
	//planeXCoef * (x0 + dx * t) + planeYCoef * (y0 + dy * t) + planeZCoef * (z0 + dz * t) + planeKCoef = 0
	//planeXCoef * x0 + planeXCoef * dx * t + planeYCoef * y0 + planeYCoef * dy * t + planeZCoef * z0 + planeZCoef * dz * t + planeKCoef = 0
	//t * (planeXCoef * dx + planeYCoef * dy + planeZCoef * dz) + planeXCoef * x0 + planeYCoef * y0 + planeZCoef * z0 + planeKCoef = 0
	//t * (planeXCoef * dx + planeYCoef * dy + planeZCoef * dz) = - (planeXCoef * x0 + planeYCoef * y0 + planeZCoef * z0 + planeKCoef)
	//t = -(planeXCoef * x0 + planeYCoef * y0 + planeZCoef * z0 + planeKCoef) / (planeXCoef * dx + planeYCoef * dy + planeZCoef * dz)

	FLOAT t = -((planeXCoef * lineA->x + planeYCoef * lineA->y + planeZCoef * lineA->z + planeKCoef) / (planeXCoef * directionX + planeYCoef * directionY + planeZCoef * directionZ));

	//Substitute t back into the parametric form to get point of intersection
	FLOAT intersectionX = lineA->x + directionX * t;
	FLOAT intersectionY = lineA->y + directionY * t;
	FLOAT intersectionZ = lineA->z + directionZ * t;

	intersection->x = intersectionX;
	intersection->y = intersectionY;
	intersection->z = intersectionZ;

	return TRUE;

}



//Load shaders
void SubspaceRender::LoadShaders() {

	ThrowIfFailed(D3DReadFileToBlob(L"./subspace_render_trianglelist_vertex.cso", &m_subspaceRenderVertexShaderTriangleList)); //Subspace render triangle list vertex shader
	ThrowIfFailed(D3DReadFileToBlob(L"./subspace_render_trianglelist_pixel.cso", &m_subspaceRenderPixelShaderTriangleList));	//Subspace render triangle list pixel shader
	ThrowIfFailed(D3DReadFileToBlob(L"./subspace_render_linelist_vertex.cso", &m_subspaceRenderVertexShaderLineList)); //Subspace render line list vertex shader
	ThrowIfFailed(D3DReadFileToBlob(L"./subspace_render_linelist_pixel.cso", &m_subspaceRenderPixelShaderLineList));	//Subspace render line list pixel shader
	ThrowIfFailed(D3DReadFileToBlob(L"./subspace_render_no_color_vertex.cso", &m_subspaceRenderVertexNoColor));	//Subspace render no color vertex shader
	ThrowIfFailed(D3DReadFileToBlob(L"./subspace_render_constant_red_pixel.cso", &m_subspaceRenderPixelConstantRed));	//Subspace render constant red pixel shader
	ThrowIfFailed(D3DReadFileToBlob(L"./subspace_render_color_normal_vertex.cso", &m_subspaceRenderVertexColorNormal));	//Subspace render color and normal vertex shader
	ThrowIfFailed(D3DReadFileToBlob(L"./subspace_render_color_normal_pixel.cso", &m_subspaceRenderPixelColorNormal));	//Subspace render color and normal pixel shader

}

//Initialize vertex buffer - triangle list
void SubspaceRender::InitVertexBufferTriangleList() {

	//Vertex buffer size -- nr_tetrahedron * 4(triangles) * 3(points) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_triangulation.size()) * 144;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = {0, (SIZE_T) vertexBufferSizeInBytes};

	m_vertexBufferTriangleList = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferTriangleList->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr = nullptr;
	BYTE* pointBPtr = nullptr;
	BYTE* pointCPtr = nullptr;
	BYTE* pointDPtr = nullptr;

	for (UINT i = 0; i < m_triangulation.size(); ++i) {

		pointAPtr = reinterpret_cast<BYTE*>(&m_triangulation[i].a.x);
		pointBPtr = reinterpret_cast<BYTE*>(&m_triangulation[i].b.x);
		pointCPtr = reinterpret_cast<BYTE*>(&m_triangulation[i].c.x);
		pointDPtr = reinterpret_cast<BYTE*>(&m_triangulation[i].d.x);

		::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointBPtr, 12); ::memcpy(uploadBufferPtr + 24, pointCPtr, 12);
		uploadBufferPtr += 36;
		::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointBPtr, 12); ::memcpy(uploadBufferPtr + 24, pointDPtr, 12);
		uploadBufferPtr += 36;
		::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointCPtr, 12); ::memcpy(uploadBufferPtr + 24, pointDPtr, 12);
		uploadBufferPtr += 36;
		::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointCPtr, 12); ::memcpy(uploadBufferPtr + 24, pointDPtr, 12);
		uploadBufferPtr += 36;

	}

	m_vertexBufferTriangleList->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferTriangleList->CopyUploadToDefault();

	m_vertexBufferViewTriangleList.BufferLocation = m_vertexBufferTriangleList->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewTriangleList.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewTriangleList.StrideInBytes = 12; // 3(coordinates) * 4(FLOAT)

}

//Initialize vertex buffer - triangle list color
void SubspaceRender::InitVertexBufferTriangleListColor() {

	//Vertex buffer size -- nr_m_voronoiColoredTriangles * 6(3 * points + 3 * color) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_voronoiColoredTriangles.size()) * 72;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferTriangleListColor = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferTriangleListColor->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr		= nullptr;
	BYTE* pointBPtr		= nullptr;
	BYTE* pointCPtr		= nullptr;
	BYTE* pointColorPtr = nullptr;

	for (UINT i = 0; i < m_voronoiColoredTriangles.size(); ++i) {

		pointAPtr = reinterpret_cast<BYTE*>(&m_voronoiColoredTriangles[i].a.x);
		pointBPtr = reinterpret_cast<BYTE*>(&m_voronoiColoredTriangles[i].b.x);
		pointCPtr = reinterpret_cast<BYTE*>(&m_voronoiColoredTriangles[i].c.x);
		pointColorPtr = reinterpret_cast<BYTE*>(&m_voronoiColoredTriangles[i].color.x);

		::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;
		::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;
		::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;

	}

	m_vertexBufferTriangleListColor->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferTriangleListColor->CopyUploadToDefault();

	m_vertexBufferViewTriangleListColor.BufferLocation = m_vertexBufferTriangleListColor->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewTriangleListColor.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewTriangleListColor.StrideInBytes = 24; // 2(point + color) * 3(coordinates) * 4(FLOAT)

}

//Initialize vertex buffer - triangle list tetrahedron color
void SubspaceRender::InitVertexBufferTriangleListTetrahedronColor() {


	//Vertex buffer size -- nr_tetrahedron * 4(triangles) * 6(3 * points + 3 * colors) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_triangulation.size()) * 288;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferTriangleListTetrahedronColor = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferTriangleListTetrahedronColor->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr = nullptr;
	BYTE* pointBPtr = nullptr;
	BYTE* pointCPtr = nullptr;
	BYTE* pointDPtr = nullptr;
	BYTE* pointColorPtr = nullptr;

	FLOAT colorR = 0.0f;
	FLOAT colorG = 0.0f;
	FLOAT colorB = 0.0f;
	Point color = {};

	for (UINT i = 0; i < m_triangulation.size(); ++i) {

		pointAPtr = reinterpret_cast<BYTE*>(&m_triangulation[i].a.x);
		pointBPtr = reinterpret_cast<BYTE*>(&m_triangulation[i].b.x);
		pointCPtr = reinterpret_cast<BYTE*>(&m_triangulation[i].c.x);
		pointDPtr = reinterpret_cast<BYTE*>(&m_triangulation[i].d.x);

		colorR = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
		colorG = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
		colorB = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;

		color = Point(colorR, colorG, colorB);

		pointColorPtr = reinterpret_cast<BYTE*>(&color.x);

		::memcpy(uploadBufferPtr, pointAPtr, 12);			::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		::memcpy(uploadBufferPtr + 24, pointBPtr, 12);		::memcpy(uploadBufferPtr + 36, pointColorPtr, 12);
		::memcpy(uploadBufferPtr + 48, pointCPtr, 12);		::memcpy(uploadBufferPtr + 60, pointColorPtr, 12);
		uploadBufferPtr += 72;
		::memcpy(uploadBufferPtr, pointAPtr, 12);			::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		::memcpy(uploadBufferPtr + 24, pointBPtr, 12);		::memcpy(uploadBufferPtr + 36, pointColorPtr, 12);
		::memcpy(uploadBufferPtr + 48, pointDPtr, 12);		::memcpy(uploadBufferPtr + 60, pointColorPtr, 12);
		uploadBufferPtr += 72;
		::memcpy(uploadBufferPtr, pointAPtr, 12);			::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		::memcpy(uploadBufferPtr + 24, pointCPtr, 12);		::memcpy(uploadBufferPtr + 36, pointColorPtr, 12);
		::memcpy(uploadBufferPtr + 48, pointDPtr, 12);		::memcpy(uploadBufferPtr + 60, pointColorPtr, 12);
		uploadBufferPtr += 72;
		::memcpy(uploadBufferPtr, pointBPtr, 12);			::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		::memcpy(uploadBufferPtr + 24, pointCPtr, 12);		::memcpy(uploadBufferPtr + 36, pointColorPtr, 12);
		::memcpy(uploadBufferPtr + 48, pointDPtr, 12);		::memcpy(uploadBufferPtr + 60, pointColorPtr, 12);
		uploadBufferPtr += 72;

	}

	m_vertexBufferTriangleListTetrahedronColor->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferTriangleListTetrahedronColor->CopyUploadToDefault();

	m_vertexBufferViewTriangleListTetrahedronColor.BufferLocation = m_vertexBufferTriangleListTetrahedronColor->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewTriangleListTetrahedronColor.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewTriangleListTetrahedronColor.StrideInBytes = 24; // 6(3 * coordinates + 3 * colors) * 4(FLOAT)


}

//Initialize vertex buffer - triangle list - clipped voronoi faces
void SubspaceRender::InitVertexBufferTriangleListClippedVoronoiFaces() {

	//Vertex buffer size -- nr_m_voronoiColoredTriangles * 6(3 * points + 3 * color) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_clippedVoronoiTriangles.size()) * 72;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferTriangleListClippedVoronoiFaces = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferTriangleListClippedVoronoiFaces->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr = nullptr;
	BYTE* pointBPtr = nullptr;
	BYTE* pointCPtr = nullptr;
	BYTE* pointColorPtr = nullptr;

	for (UINT i = 0; i < m_clippedVoronoiTriangles.size(); ++i) {

		pointAPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiTriangles[i].a.x);
		pointBPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiTriangles[i].b.x);
		pointCPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiTriangles[i].c.x);
		pointColorPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiTriangles[i].color.x);

		::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;
		::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;
		::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;

	}

	m_vertexBufferTriangleListClippedVoronoiFaces->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferTriangleListClippedVoronoiFaces->CopyUploadToDefault();

	m_vertexBufferViewTriangleListClippedVoronoiFaces.BufferLocation = m_vertexBufferTriangleListClippedVoronoiFaces->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewTriangleListClippedVoronoiFaces.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewTriangleListClippedVoronoiFaces.StrideInBytes = 24; // 2(point + color) * 3(coordinates) * 4(FLOAT)

}

//Initialize vertex buffer - triangle list - clipped voronoi cells
void SubspaceRender::InitVertexBufferTriangleListClippedVoronoiCells() {

	//Vertex buffer size -- nr_m_clippedVoronoiCellsTriangles * 6(3 * points + 3 * color) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_clippedVoronoiCellsBackCulledTriangles.size()) * 72;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferTriangleListClippedVoronoiCellsBackCull = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferTriangleListClippedVoronoiCellsBackCull->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr = nullptr;
	BYTE* pointBPtr = nullptr;
	BYTE* pointCPtr = nullptr;
	BYTE* pointColorPtr = nullptr;

	for (UINT i = 0; i < m_clippedVoronoiCellsBackCulledTriangles.size(); ++i) {

		pointAPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiCellsBackCulledTriangles[i].a.x);
		pointBPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiCellsBackCulledTriangles[i].b.x);
		pointCPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiCellsBackCulledTriangles[i].c.x);
		pointColorPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiCellsBackCulledTriangles[i].color.x);

		::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;
		::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;
		::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;

	}

	m_vertexBufferTriangleListClippedVoronoiCellsBackCull->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferTriangleListClippedVoronoiCellsBackCull->CopyUploadToDefault();

	m_vertexBufferViewTriangleListClippedVoronoiCellsBackCull.BufferLocation = m_vertexBufferTriangleListClippedVoronoiCellsBackCull->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewTriangleListClippedVoronoiCellsBackCull.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewTriangleListClippedVoronoiCellsBackCull.StrideInBytes = 24; // 2(point + color) * 3(coordinates) * 4(FLOAT)

}

//Initialize vertex buffer - triangle list - clipped voronoi cells back culled without faces that lie on the unit cube
void SubspaceRender::InitVertexBufferTriangleListClippedVoronoiCellsBackCull() {

	//Vertex buffer size -- nr_m_clippedVoronoiCellsTriangles * 6(3 * points + 3 * color) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_clippedVoronoiCellsTriangles.size()) * 72;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferTriangleListClippedVoronoiCells = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferTriangleListClippedVoronoiCells->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr = nullptr;
	BYTE* pointBPtr = nullptr;
	BYTE* pointCPtr = nullptr;
	BYTE* pointColorPtr = nullptr;

	for (UINT i = 0; i < m_clippedVoronoiCellsTriangles.size(); ++i) {

		pointAPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiCellsTriangles[i].a.x);
		pointBPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiCellsTriangles[i].b.x);
		pointCPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiCellsTriangles[i].c.x);
		pointColorPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiCellsTriangles[i].color.x);

		::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;
		::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;
		::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;

	}

	m_vertexBufferTriangleListClippedVoronoiCells->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferTriangleListClippedVoronoiCells->CopyUploadToDefault();

	m_vertexBufferViewTriangleListClippedVoronoiCells.BufferLocation = m_vertexBufferTriangleListClippedVoronoiCells->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewTriangleListClippedVoronoiCells.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewTriangleListClippedVoronoiCells.StrideInBytes = 24; // 2(point + color) * 3(coordinates) * 4(FLOAT)

}

//Initialize vertex buffer - triangle list - clipped voronoi cells back culled with face normals
void SubspaceRender::InitVertexBufferTriangleListClippedVoronoiCellsBackCullNormals() {

	//Vertex buffer size -- nr_m_clippedVoronoiCellsBackCulledTrianglesNormals * 9(3 * points + 3 * color + 3 * normal) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_clippedVoronoiCellsBackCulledTrianglesNormals.size()) * 108;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferTriangleListClippedVoronoiCellsBackCullNormals = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferTriangleListClippedVoronoiCellsBackCullNormals->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr = nullptr;
	BYTE* pointBPtr = nullptr;
	BYTE* pointCPtr = nullptr;
	BYTE* pointColorPtr = nullptr;
	BYTE* pointNormalPtr = nullptr;

	for (UINT i = 0; i < m_clippedVoronoiCellsBackCulledTrianglesNormals.size(); ++i) {

		pointAPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiCellsBackCulledTrianglesNormals[i].a.x);
		pointBPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiCellsBackCulledTrianglesNormals[i].b.x);
		pointCPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiCellsBackCulledTrianglesNormals[i].c.x);
		pointColorPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiCellsBackCulledTrianglesNormals[i].color.x);
		pointNormalPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiCellsBackCulledTrianglesNormals[i].normal.x);

		::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
		uploadBufferPtr += 36;
		::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
		uploadBufferPtr += 36;
		::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
		uploadBufferPtr += 36;

	}

	m_vertexBufferTriangleListClippedVoronoiCellsBackCullNormals->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferTriangleListClippedVoronoiCellsBackCullNormals->CopyUploadToDefault();

	m_vertexBufferViewTriangleListClippedVoronoiCellsBackCullNormals.BufferLocation = m_vertexBufferTriangleListClippedVoronoiCellsBackCullNormals->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewTriangleListClippedVoronoiCellsBackCullNormals.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewTriangleListClippedVoronoiCellsBackCullNormals.StrideInBytes = 36; // 3(point + color + normal) * 3(coordinates) * 4(FLOAT)

}

//Initialize vertex buffer - triangle list - clipped voronoi faces with face normals
void SubspaceRender::InitVertexBufferTriangleListClippedVoronoiFacesNormals() {

	//Vertex buffer size -- nr_m_clippedVoronoiTrianglesFaceNormals * 9(3 * points + 3 * color + 3 * normal) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_clippedVoronoiTrianglesFaceNormals.size()) * 108;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferTriangleListClippedVoronoiFacesNormals = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferTriangleListClippedVoronoiFacesNormals->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr = nullptr;
	BYTE* pointBPtr = nullptr;
	BYTE* pointCPtr = nullptr;
	BYTE* pointColorPtr = nullptr;
	BYTE* pointNormalPtr = nullptr;

	for (UINT i = 0; i < m_clippedVoronoiTrianglesFaceNormals.size(); ++i) {

		pointAPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiTrianglesFaceNormals[i].a.x);
		pointBPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiTrianglesFaceNormals[i].b.x);
		pointCPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiTrianglesFaceNormals[i].c.x);
		pointColorPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiTrianglesFaceNormals[i].color.x);
		pointNormalPtr = reinterpret_cast<BYTE*>(&m_clippedVoronoiTrianglesFaceNormals[i].normal.x);

		::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
		uploadBufferPtr += 36;
		::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
		uploadBufferPtr += 36;
		::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
		uploadBufferPtr += 36;

	}

	m_vertexBufferTriangleListClippedVoronoiFacesNormals->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferTriangleListClippedVoronoiFacesNormals->CopyUploadToDefault();

	m_vertexBufferViewTriangleListClippedVoronoiFacesNormals.BufferLocation = m_vertexBufferTriangleListClippedVoronoiFacesNormals->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewTriangleListClippedVoronoiFacesNormals.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewTriangleListClippedVoronoiFacesNormals.StrideInBytes = 36; // 3(point + color + normal) * 3(coordinates) * 4(FLOAT)


}

//Initialize vertex buffer - line list
void SubspaceRender::InitVertexBufferLineList() {

	//Vertex buffer size -- nr_edges * 4(2 * points + 2 * colors) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_voronoiColoredEdges.size()) * 48;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferLineList = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferLineList->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr		= nullptr;
	BYTE* pointBPtr		= nullptr;
	BYTE* pointColorPtr = nullptr;

	for (UINT i = 0; i < m_voronoiColoredEdges.size(); ++i) {

		pointAPtr		= reinterpret_cast<BYTE*>(&m_voronoiColoredEdges[i].a.x);
		pointBPtr		= reinterpret_cast<BYTE*>(&m_voronoiColoredEdges[i].b.x);
		pointColorPtr	= reinterpret_cast<BYTE*>(&m_voronoiColoredEdges[i].color.x);

		::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;
		::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;

	}

	m_vertexBufferLineList->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferLineList->CopyUploadToDefault();

	m_vertexBufferViewLineList.BufferLocation = m_vertexBufferLineList->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewLineList.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewLineList.StrideInBytes = 24; // 2(point + color) * 3(coordinates) * 4(FLOAT)

}

//Initialize vertex buffer - line list - delaunay edges (only connecting centroids)
void SubspaceRender::InitVertexBufferLineListDelaunayEdgesColor() {

	//Vertex buffer size -- nr_edges * 4(2 * points + 2 * colors) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_delaunayEdges.size()) * 48;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferLineListDelaunayEdgesColor = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferLineListDelaunayEdgesColor->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr = nullptr;
	BYTE* pointBPtr = nullptr;
	BYTE* pointColorPtr = nullptr;

	FLOAT colorR = 0.0f;
	FLOAT colorG = 0.0f;
	FLOAT colorB = 0.0f;
	Point color = {};

	for (UINT i = 0; i < m_delaunayEdges.size(); ++i) {

		pointAPtr = reinterpret_cast<BYTE*>(&m_delaunayEdges[i].a.x);
		pointBPtr = reinterpret_cast<BYTE*>(&m_delaunayEdges[i].b.x);

		colorR = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
		colorG = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;
		colorB = ((FLOAT)std::rand()) / (FLOAT)RAND_MAX;

		//color = Point(colorR, colorG, colorB);
		color = Point(1.0f, 1.0f, 0.0f);

		pointColorPtr = reinterpret_cast<BYTE*>(&color.x);

		::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;
		::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;

	}

	m_vertexBufferLineListDelaunayEdgesColor->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferLineListDelaunayEdgesColor->CopyUploadToDefault();

	m_vertexBufferViewLineListDelaunayEdgesColor.BufferLocation = m_vertexBufferLineListDelaunayEdgesColor->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewLineListDelaunayEdgesColor.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewLineListDelaunayEdgesColor.StrideInBytes = 24; // 2(point + color) * 3(coordinates) * 4(FLOAT)

}

//Initialize vertex buffer - line list no color
void SubspaceRender::InitVertexBufferLineListNoColor() {

	//Vertex buffer size -- nr_edges * 2(points) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_voronoiEdges.size()) * 24;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferLineListNoColor = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferLineListNoColor->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr = nullptr;
	BYTE* pointBPtr = nullptr;

	for (UINT i = 0; i < m_voronoiEdges.size(); ++i) {

		pointAPtr = reinterpret_cast<BYTE*>(&m_voronoiEdges[i].a.x);
		pointBPtr = reinterpret_cast<BYTE*>(&m_voronoiEdges[i].b.x);
		
		::memcpy(uploadBufferPtr, pointAPtr, 12);
		uploadBufferPtr += 12;
		::memcpy(uploadBufferPtr, pointBPtr, 12);
		uploadBufferPtr += 12;

	}

	m_vertexBufferLineListNoColor->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferLineListNoColor->CopyUploadToDefault();

	m_vertexBufferViewLineListNoColor.BufferLocation = m_vertexBufferLineListNoColor->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewLineListNoColor.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewLineListNoColor.StrideInBytes = 12; // 3(coordinates) * 4(FLOAT)

}

//Initialize vertex buffer - line list no color - voronoi faces (voronoi edges)
void SubspaceRender::InitVertexBufferLineListVoronoiFacesNoColor() {

	//Vertex buffer size -- nr_edges * 2(points) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_voronoiFaces.size()) * 24;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferLineListVoronoiFacesNoColor = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferLineListVoronoiFacesNoColor->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr = nullptr;
	BYTE* pointBPtr = nullptr;

	for (UINT i = 0; i < m_voronoiFaces.size(); ++i) {

		pointAPtr = reinterpret_cast<BYTE*>(&m_voronoiFaces[i].a.x);
		pointBPtr = reinterpret_cast<BYTE*>(&m_voronoiFaces[i].b.x);

		::memcpy(uploadBufferPtr, pointAPtr, 12);
		uploadBufferPtr += 12;
		::memcpy(uploadBufferPtr, pointBPtr, 12);
		uploadBufferPtr += 12;

	}

	m_vertexBufferLineListVoronoiFacesNoColor->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferLineListVoronoiFacesNoColor->CopyUploadToDefault();

	m_vertexBufferViewLineListVoronoiFacesNoColor.BufferLocation = m_vertexBufferLineListVoronoiFacesNoColor->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewLineListVoronoiFacesNoColor.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewLineListVoronoiFacesNoColor.StrideInBytes = 12; // 3(coordinates) * 4(FLOAT)


}

//Initialize vertex buffer - line list no color - unit cube edges
void SubspaceRender::InitVertexBufferLineListUnitCubeNoColor() {

	//Vertex buffer size -- nr_edges * 2(points) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_unitCubeEdges.size()) * 24;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferLineListUnitCubeNoColor = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferLineListUnitCubeNoColor->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr = nullptr;
	BYTE* pointBPtr = nullptr;

	for (UINT i = 0; i < m_unitCubeEdges.size(); ++i) {

		pointAPtr = reinterpret_cast<BYTE*>(&m_unitCubeEdges[i].a.x);
		pointBPtr = reinterpret_cast<BYTE*>(&m_unitCubeEdges[i].b.x);

		::memcpy(uploadBufferPtr, pointAPtr, 12);
		uploadBufferPtr += 12;
		::memcpy(uploadBufferPtr, pointBPtr, 12);
		uploadBufferPtr += 12;

	}

	m_vertexBufferLineListUnitCubeNoColor->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferLineListUnitCubeNoColor->CopyUploadToDefault();

	m_vertexBufferViewLineListUnitCubeNoColor.BufferLocation = m_vertexBufferLineListUnitCubeNoColor->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewLineListUnitCubeNoColor.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewLineListUnitCubeNoColor.StrideInBytes = 12; // 3(coordinates) * 4(FLOAT)

}

//Initialize vertex buffer - line list - voronoi edges, including those that lie on the unit cube
void SubspaceRender::InitVertexBufferLineListVoronoiEdgesUnitCubeColor() {

	//Vertex buffer size -- nr_edges * 4(2 * points + 2 * colors) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = static_cast<UINT64>(m_voronoiEdgesUnitCube.size()) * 48;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferLineListVoronoiEdgesUnitCubeColor = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferLineListVoronoiEdgesUnitCubeColor->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr = nullptr;
	BYTE* pointBPtr = nullptr;
	BYTE* pointColorPtr = nullptr;

	for (UINT i = 0; i < m_voronoiEdgesUnitCube.size(); ++i) {

		pointAPtr = reinterpret_cast<BYTE*>(&m_voronoiEdgesUnitCube[i].a.x);
		pointBPtr = reinterpret_cast<BYTE*>(&m_voronoiEdgesUnitCube[i].b.x);
		pointColorPtr = reinterpret_cast<BYTE*>(&m_voronoiEdgesUnitCube[i].color.x);

		::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;
		::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12);
		uploadBufferPtr += 24;

	}

	m_vertexBufferLineListVoronoiEdgesUnitCubeColor->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferLineListVoronoiEdgesUnitCubeColor->CopyUploadToDefault();

	m_vertexBufferViewLineListVoronoiEdgesUnitCubeColor.BufferLocation = m_vertexBufferLineListVoronoiEdgesUnitCubeColor->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewLineListVoronoiEdgesUnitCubeColor.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewLineListVoronoiEdgesUnitCubeColor.StrideInBytes = 24; // 2(point + color) * 3(coordinates) * 4(FLOAT)

}

//Initialize depth buffer
void SubspaceRender::InitDepthBuffer() {

	//Create dsv descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDescription = {};
	descriptorHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descriptorHeapDescription.NumDescriptors = 1;
	descriptorHeapDescription.NodeMask = 0;
	descriptorHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(m_dxDevice->CreateDescriptorHeap(&descriptorHeapDescription, IID_PPV_ARGS(&m_dsvDescriptorHeap)));

	//Create depth buffer
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil = { 1.0f, 0 };

	m_depthBuffer = std::make_unique<Resource>(m_dxDevice, m_copyCQ, RESOURCE_NO_READBACK | RESOURCE_NO_UPLOAD, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		WINDOW_WIDTH, WINDOW_HEIGHT, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, SUBSPACERENDER_DEPTH_STENCIL_BUFFER_FORMAT, 1, 0, D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &clearValue);

	//Set depth buffer descriptor
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription = {};
	depthStencilViewDescription.Format = SUBSPACERENDER_DEPTH_STENCIL_BUFFER_FORMAT;
	depthStencilViewDescription.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDescription.Texture2D.MipSlice = 0;
	depthStencilViewDescription.Flags = D3D12_DSV_FLAG_NONE;

	m_dxDevice->CreateDepthStencilView(m_depthBuffer->GetDefaultBuffer().Get(), &depthStencilViewDescription, m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

}

//Initialize root signature
void SubspaceRender::InitRootSignature() {

	D3D12_ROOT_PARAMETER1 rootParameters[1] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[0].Constants.Num32BitValues = sizeof(DirectX::XMMATRIX) / 4;
	rootParameters[0].Constants.RegisterSpace = 0;
	rootParameters[0].Constants.ShaderRegister = 0;

	m_rootSignature = std::make_unique<RootSignature>(m_dxDevice, &m_rootSignatureVersionSupport, 
		&rootParameters[0], static_cast<UINT>(_countof(rootParameters)), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

}

//Initialize root signature - lighting
void SubspaceRender::InitRootSignatureLighting() {

	D3D12_ROOT_PARAMETER1 rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[0].Constants.Num32BitValues = sizeof(DirectX::XMMATRIX) / 4;
	rootParameters[0].Constants.RegisterSpace = 0;
	rootParameters[0].Constants.ShaderRegister = 0;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Constants.Num32BitValues = sizeof(DirectX::XMMATRIX) / 4;
	rootParameters[1].Constants.RegisterSpace = 0;
	rootParameters[1].Constants.ShaderRegister = 1;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[2].Constants.Num32BitValues = sizeof(DirectX::XMMATRIX) / 4;
	rootParameters[2].Constants.RegisterSpace = 0;
	rootParameters[2].Constants.ShaderRegister = 2;

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Constants.Num32BitValues = sizeof(DirectX::XMVECTOR) / 4;
	rootParameters[3].Constants.RegisterSpace = 0;
	rootParameters[3].Constants.ShaderRegister = 0;

	m_rootSignatureLighting = std::make_unique<RootSignature>(m_dxDevice, &m_rootSignatureVersionSupport,
		rootParameters, static_cast<UINT>(_countof(rootParameters)), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

}

//Initialize pipeline state object - triangle list with no color and rasterizer set to wireframe
void SubspaceRender::InitPipelineStateObjectTriangleList() {
	
	//Input assembler layout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDescription = {};
	D3D12_INPUT_ELEMENT_DESC inputLayoutElementsDescription[] = {

			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

	};
	inputLayoutDescription.pInputElementDescs = inputLayoutElementsDescription;
	inputLayoutDescription.NumElements = _countof(inputLayoutElementsDescription);

	//Vertex shader
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = m_subspaceRenderVertexShaderTriangleList->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = m_subspaceRenderVertexShaderTriangleList->GetBufferPointer();

	//Pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = m_subspaceRenderPixelShaderTriangleList->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = m_subspaceRenderPixelShaderTriangleList->GetBufferPointer();

	//Render target formats
	D3D12_RT_FORMAT_ARRAY renderTargetFormats = {};
	renderTargetFormats.RTFormats[0] = SWAP_CHAIN_BUFFER_FORMAT;
	renderTargetFormats.NumRenderTargets = 1;

	//Rasterizer
	CD3DX12_RASTERIZER_DESC rasterizerDescription = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_WIREFRAME, D3D12_CULL_MODE_NONE,
		FALSE, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, FALSE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

	//Blending
	D3D12_BLEND_DESC blendDescription = {};
	blendDescription.AlphaToCoverageEnable = FALSE;
	blendDescription.IndependentBlendEnable = FALSE;
	blendDescription.RenderTarget[0].BlendEnable = FALSE;
	blendDescription.RenderTarget[0].LogicOpEnable = FALSE;
	blendDescription.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDescription.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	CD3DX12_BLEND_DESC cd3dx12_blendDescription = CD3DX12_BLEND_DESC(blendDescription);

	m_pipelineStateTriangleList = std::make_unique<PipelineState>(m_dxDevice, &inputLayoutDescription, m_rootSignature->GetRootSignature(), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		&vertexShaderBytecode, &pixelShaderBytecode, DXGI_FORMAT_D32_FLOAT, &renderTargetFormats, &rasterizerDescription, &cd3dx12_blendDescription);
	
}

//Initialize pipeline state object - triangle list with color input and rasterizer set to fill
void SubspaceRender::InitPipelineStateObjectTriangleListColor() {

	//Input assembler layout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDescription = {};
	D3D12_INPUT_ELEMENT_DESC inputLayoutElementsDescription[] = {

			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

	};
	inputLayoutDescription.pInputElementDescs = inputLayoutElementsDescription;
	inputLayoutDescription.NumElements = _countof(inputLayoutElementsDescription);

	//Vertex shader
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = m_subspaceRenderVertexShaderLineList->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = m_subspaceRenderVertexShaderLineList->GetBufferPointer();

	//Pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = m_subspaceRenderPixelShaderLineList->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = m_subspaceRenderPixelShaderLineList->GetBufferPointer();

	//Render target formats
	D3D12_RT_FORMAT_ARRAY renderTargetFormats = {};
	renderTargetFormats.RTFormats[0] = SWAP_CHAIN_BUFFER_FORMAT;
	renderTargetFormats.NumRenderTargets = 1;

	//Rasterizer
	CD3DX12_RASTERIZER_DESC rasterizerDescription = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE,
		FALSE, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, FALSE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

	//Blending
	D3D12_BLEND_DESC blendDescription = {};
	blendDescription.AlphaToCoverageEnable = FALSE;
	blendDescription.IndependentBlendEnable = FALSE;
	blendDescription.RenderTarget[0].BlendEnable = FALSE;
	blendDescription.RenderTarget[0].LogicOpEnable = FALSE;
	blendDescription.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDescription.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	CD3DX12_BLEND_DESC cd3dx12_blendDescription = CD3DX12_BLEND_DESC(blendDescription);

	m_pipelineStateTriangleListColor = std::make_unique<PipelineState>(m_dxDevice, &inputLayoutDescription, m_rootSignature->GetRootSignature(), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		&vertexShaderBytecode, &pixelShaderBytecode, DXGI_FORMAT_D32_FLOAT, &renderTargetFormats, &rasterizerDescription, &cd3dx12_blendDescription);

}

//Initialize pipeline state object - triangle list with color input and rasterizer set to fill, back cull and blend enabled 
void SubspaceRender::InitPipelineStateObjectTriangleListColorBackCull() {

	//Input assembler layout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDescription = {};
	D3D12_INPUT_ELEMENT_DESC inputLayoutElementsDescription[] = {

			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

	};
	inputLayoutDescription.pInputElementDescs = inputLayoutElementsDescription;
	inputLayoutDescription.NumElements = _countof(inputLayoutElementsDescription);

	//Vertex shader
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = m_subspaceRenderVertexShaderLineList->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = m_subspaceRenderVertexShaderLineList->GetBufferPointer();

	//Pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = m_subspaceRenderPixelShaderLineList->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = m_subspaceRenderPixelShaderLineList->GetBufferPointer();

	//Render target formats
	D3D12_RT_FORMAT_ARRAY renderTargetFormats = {};
	renderTargetFormats.RTFormats[0] = SWAP_CHAIN_BUFFER_FORMAT;
	renderTargetFormats.NumRenderTargets = 1;

	//Rasterizer
	CD3DX12_RASTERIZER_DESC rasterizerDescription = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_FRONT,
		FALSE, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, FALSE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

	//Blending
	D3D12_BLEND_DESC blendDescription = {};
	blendDescription.AlphaToCoverageEnable = FALSE;
	blendDescription.IndependentBlendEnable = FALSE;
	blendDescription.RenderTarget[0].BlendEnable = TRUE;
	blendDescription.RenderTarget[0].LogicOpEnable = FALSE;
	blendDescription.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_COLOR;
	blendDescription.RenderTarget[0].DestBlend = D3D12_BLEND_BLEND_FACTOR;
	blendDescription.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDescription.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	CD3DX12_BLEND_DESC cd3dx12_blendDescription = CD3DX12_BLEND_DESC(blendDescription);

	m_pipelineStateTriangleListColorBackCull = std::make_unique<PipelineState>(m_dxDevice, &inputLayoutDescription, m_rootSignature->GetRootSignature(), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		&vertexShaderBytecode, &pixelShaderBytecode, DXGI_FORMAT_D32_FLOAT, &renderTargetFormats, &rasterizerDescription, &cd3dx12_blendDescription);

}

//Initialize pipeline state object - triangle list with color and face normal input, raseterizer set to full and back cull, blend enabled
void SubspaceRender::InitPipelineStateObjectTriangleListColorBackCullNormal() {

	//Input assembler layout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDescription = {};
	D3D12_INPUT_ELEMENT_DESC inputLayoutElementsDescription[] = {

			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

	};
	inputLayoutDescription.pInputElementDescs = inputLayoutElementsDescription;
	inputLayoutDescription.NumElements = _countof(inputLayoutElementsDescription);

	//Vertex shader
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = m_subspaceRenderVertexColorNormal->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = m_subspaceRenderVertexColorNormal->GetBufferPointer();

	//Pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = m_subspaceRenderPixelColorNormal->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = m_subspaceRenderPixelColorNormal->GetBufferPointer();

	//Render target formats
	D3D12_RT_FORMAT_ARRAY renderTargetFormats = {};
	renderTargetFormats.RTFormats[0] = SWAP_CHAIN_BUFFER_FORMAT;
	renderTargetFormats.NumRenderTargets = 1;

	//Rasterizer
	CD3DX12_RASTERIZER_DESC rasterizerDescription = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_FRONT,
		FALSE, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, FALSE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

	//Blending
	D3D12_BLEND_DESC blendDescription = {};
	blendDescription.AlphaToCoverageEnable = FALSE;
	blendDescription.IndependentBlendEnable = FALSE;
	blendDescription.RenderTarget[0].BlendEnable = TRUE;
	blendDescription.RenderTarget[0].LogicOpEnable = FALSE;
	blendDescription.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_COLOR;
	blendDescription.RenderTarget[0].DestBlend = D3D12_BLEND_BLEND_FACTOR;
	blendDescription.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDescription.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	CD3DX12_BLEND_DESC cd3dx12_blendDescription = CD3DX12_BLEND_DESC(blendDescription);

	m_pipelineStateTriangleListColorBackCullNormal = std::make_unique<PipelineState>(m_dxDevice, &inputLayoutDescription, m_rootSignatureLighting->GetRootSignature(), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		&vertexShaderBytecode, &pixelShaderBytecode, DXGI_FORMAT_D32_FLOAT, &renderTargetFormats, &rasterizerDescription, &cd3dx12_blendDescription);

}

//Initialize pipeline state object - triangle list with color and face normal input and rasterizer set to fill
void SubspaceRender::InitPipelineStateObjectTriangleListColorNormal() {

	//Input assembler layout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDescription = {};
	D3D12_INPUT_ELEMENT_DESC inputLayoutElementsDescription[] = {

			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

	};
	inputLayoutDescription.pInputElementDescs = inputLayoutElementsDescription;
	inputLayoutDescription.NumElements = _countof(inputLayoutElementsDescription);

	//Vertex shader
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = m_subspaceRenderVertexColorNormal->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = m_subspaceRenderVertexColorNormal->GetBufferPointer();

	//Pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = m_subspaceRenderPixelColorNormal->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = m_subspaceRenderPixelColorNormal->GetBufferPointer();

	//Render target formats
	D3D12_RT_FORMAT_ARRAY renderTargetFormats = {};
	renderTargetFormats.RTFormats[0] = SWAP_CHAIN_BUFFER_FORMAT;
	renderTargetFormats.NumRenderTargets = 1;

	//Rasterizer
	CD3DX12_RASTERIZER_DESC rasterizerDescription = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE,
		FALSE, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, FALSE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

	//Blending
	D3D12_BLEND_DESC blendDescription = {};
	blendDescription.AlphaToCoverageEnable = FALSE;
	blendDescription.IndependentBlendEnable = FALSE;
	blendDescription.RenderTarget[0].BlendEnable = FALSE;
	blendDescription.RenderTarget[0].LogicOpEnable = FALSE;
	blendDescription.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDescription.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	CD3DX12_BLEND_DESC cd3dx12_blendDescription = CD3DX12_BLEND_DESC(blendDescription);

	m_pipelineStateTriangleListColorNormal = std::make_unique<PipelineState>(m_dxDevice, &inputLayoutDescription, m_rootSignatureLighting->GetRootSignature(), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		&vertexShaderBytecode, &pixelShaderBytecode, DXGI_FORMAT_D32_FLOAT, &renderTargetFormats, &rasterizerDescription, &cd3dx12_blendDescription);

}

//Initialize pipeline state object - line list
void SubspaceRender::InitPipelineStateObjectLineList() {

	//Input assembler layout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDescription = {};
	D3D12_INPUT_ELEMENT_DESC inputLayoutElementsDescription[] = {

			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

	};
	inputLayoutDescription.pInputElementDescs = inputLayoutElementsDescription;
	inputLayoutDescription.NumElements = _countof(inputLayoutElementsDescription);

	//Vertex shader
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = m_subspaceRenderVertexShaderLineList->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = m_subspaceRenderVertexShaderLineList->GetBufferPointer();

	//Pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = m_subspaceRenderPixelShaderLineList->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = m_subspaceRenderPixelShaderLineList->GetBufferPointer();

	//Render target formats
	D3D12_RT_FORMAT_ARRAY renderTargetFormats = {};
	renderTargetFormats.RTFormats[0] = SWAP_CHAIN_BUFFER_FORMAT;
	renderTargetFormats.NumRenderTargets = 1;

	//Rasterizer
	CD3DX12_RASTERIZER_DESC rasterizerDescription = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_WIREFRAME, D3D12_CULL_MODE_NONE,
		FALSE, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, FALSE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

	//Blending
	D3D12_BLEND_DESC blendDescription = {};
	blendDescription.AlphaToCoverageEnable = FALSE;
	blendDescription.IndependentBlendEnable = FALSE;
	blendDescription.RenderTarget[0].BlendEnable = FALSE;
	blendDescription.RenderTarget[0].LogicOpEnable = FALSE;
	blendDescription.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDescription.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	CD3DX12_BLEND_DESC cd3dx12_blendDescription = CD3DX12_BLEND_DESC(blendDescription);

	m_pipelineStateLineList = std::make_unique<PipelineState>(m_dxDevice, &inputLayoutDescription, m_rootSignature->GetRootSignature(), D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
		&vertexShaderBytecode, &pixelShaderBytecode, DXGI_FORMAT_D32_FLOAT, &renderTargetFormats, &rasterizerDescription, &cd3dx12_blendDescription);


}

//Initialize pipeline state object - line list with no color in the input assembler
void SubspaceRender::InitPipelineStateObjectLineListNoColor() {

	//Input assembler layout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDescription = {};
	D3D12_INPUT_ELEMENT_DESC inputLayoutElementsDescription[] = {

			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

	};
	inputLayoutDescription.pInputElementDescs = inputLayoutElementsDescription;
	inputLayoutDescription.NumElements = _countof(inputLayoutElementsDescription);

	//Vertex shader
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = m_subspaceRenderVertexNoColor->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = m_subspaceRenderVertexNoColor->GetBufferPointer();

	//Pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = m_subspaceRenderPixelConstantRed->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = m_subspaceRenderPixelConstantRed->GetBufferPointer();

	//Render target formats
	D3D12_RT_FORMAT_ARRAY renderTargetFormats = {};
	renderTargetFormats.RTFormats[0] = SWAP_CHAIN_BUFFER_FORMAT;
	renderTargetFormats.NumRenderTargets = 1;

	//Rasterizer
	CD3DX12_RASTERIZER_DESC rasterizerDescription = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_WIREFRAME, D3D12_CULL_MODE_NONE,
		FALSE, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, FALSE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

	//Blending
	D3D12_BLEND_DESC blendDescription = {};
	blendDescription.AlphaToCoverageEnable = FALSE;
	blendDescription.IndependentBlendEnable = FALSE;
	blendDescription.RenderTarget[0].BlendEnable = FALSE;
	blendDescription.RenderTarget[0].LogicOpEnable = FALSE;
	blendDescription.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDescription.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	CD3DX12_BLEND_DESC cd3dx12_blendDescription = CD3DX12_BLEND_DESC(blendDescription);

	m_pipelineStateLineListNoColor = std::make_unique<PipelineState>(m_dxDevice, &inputLayoutDescription, m_rootSignature->GetRootSignature(), D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
		&vertexShaderBytecode, &pixelShaderBytecode, DXGI_FORMAT_D32_FLOAT, &renderTargetFormats, &rasterizerDescription, &cd3dx12_blendDescription);


}

//Make unit vector from x and y mouse screen coordinates
DirectX::XMVECTOR SubspaceRender::GetUnitVector(WORD mX, WORD mY) {

	FLOAT vectorX = 0.0f;
	FLOAT vectorY = 0.0f;
	FLOAT vectorZ = 0.0f;
	FLOAT xySquared = 0.0f;

	auto sphereDiameter = min(WINDOW_WIDTH, WINDOW_HEIGHT);
	FLOAT sphereRadius = static_cast<FLOAT>(sphereDiameter) / 2.0f;

	//Not normalized coordinates
	vectorX = (FLOAT)mX - (FLOAT)(WINDOW_WIDTH / 2);
	vectorY = (FLOAT)(WINDOW_HEIGHT - mY) - (FLOAT)(WINDOW_HEIGHT / 2);

	//Cap coordinates
	if (abs(vectorX) > sphereRadius) {

		if (vectorX < 0.0f) vectorX = -sphereRadius;
		else vectorX = sphereRadius;

	}

	if (abs(vectorY) > sphereRadius) {

		if (vectorY < 0.0f) vectorY = -sphereRadius;
		else vectorY = sphereRadius;

	}

	//Normalized coordinates
	vectorX = vectorX / sphereRadius;
	vectorY = vectorY / sphereRadius;

	//Get Z coordinate
	xySquared = vectorX * vectorX + vectorY * vectorY;
	if (xySquared > 1.0f) {

		DirectX::XMVECTOR returnVector = DirectX::XMVector3Normalize(DirectX::XMVectorSet(vectorX, vectorY, 0.0f, 0.0f));
		return returnVector;

	}

	vectorZ = -sqrtf(1.0f - xySquared);
	return DirectX::XMVectorSet(vectorX, vectorY, vectorZ, 0.0f);

}


