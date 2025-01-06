## Voronoi Tessselation on a Cube

* ``git clone`` this repository
* Navigate to ``/``
* Create ``build`` directory
* Navigate to ``/build``
* Open developer command prompt for VS
* Run ``cmake ../``
* Run ``cmake --build . --target install``
* Navigate to ``/``
* Create ``media`` directory
* Navigate to ``/media``
* Copy a ``1280 x 720`` resolution, ``H.264`` encoded, ``.mkv`` matroska container video file to ``/media``
* Navigate to ``/``
* Run ``main.exe``

## Switching out the Renderer

* Navigate to ``/source``
* Open ``application.cpp`` in a file editor
* Navigate to the ``Application::Run()`` function
* On the line ``m_renderer = std::make_unique<***>(m_dxDevice, m_copyCQ, m_directCQ)`` replace *** with either SubspaceRender, CIterationsRender or CQRender
* Navigate to ``/build``
* Run ``cmake --build . --target install``