//#############################################################################
                                 /*-:::--.`
                            -+shmMMNmmddmmNMNmho/.
                 `yyo++osddMms:                  `/yNNy-
              yo    +mMy:                       `./dMMdyssssso-
              oy  -dMy.                     `-+ssso:.`:mMy`   .ys
                ho+MN:                  `:osso/.         oMm-   +h
                +Mmd-           `/syo-                     :MNhs`
                `NM-.hs`      :syo:                          sMh
                oMh   :ho``/yy+.                             `MM.
                hM+    `yNN/`                                 dM+
                dM/  -sy/`/ho`                                hMo
                hMo/ho.     :yy-                             dM/
            :dNM/             :yy:                         yMy
            sy`:MN.              `+ys-                     +Mm`
            oy`   :NM+                  .+ys/`           `hMd.ys
            /sssssyNMm:                   `:sys:`     `oNN+   m-
                        .sNMh+.                   `:sNMdyysssssy:
                        -odMNhs+:-.`    `.-/oydMNh+.
                            `-+shdNMMMMMMMNmdyo/.
                                    `````*/
//#############################################################################
// Pipeline to process our PointCloud data
//#############################################################################

#ifndef pipeline_H_
#define pipeline_H_

//
// Our pipeline is based on the input from a realsense data and the extra
// metadata captured by our capturing system.
//
// This API is experimental and we are still defining the functionality and
// and roadmap.
//
// On our requirements list we have:
//
// - Offline - UI less processing of data, generating JSON outputs that can be
// digested by a Geo located database
//
// - Color and Infrared processing
// - Hyperspectral processing
// - Geo located object data
// - Time located object data
//
// - Farm specific data
//	 . Plant
//	 . Row
//   . Field
//
//	 . Analyse
//		Growth
//		Health
//

//-----------------------------------------------------------------------------
// SYSTEM
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <mutex>
#include <iostream>
#include <random>

//-----------------------------------------------------------------------------
// LIBREALSENSE
//-----------------------------------------------------------------------------

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API

//-----------------------------------------------------------------------------
// JSON processing
//-----------------------------------------------------------------------------

// Examples https://github.com/nlohmann/json#examples
#include "json/json.hpp"

#define PCL_NO_PRECOMPILE

//-----------------------------------------------------------------------------
// PCL System
//-----------------------------------------------------------------------------

#include <pcl/point_cloud.h>
#include <pcl/common/io.h>

#include <pcl/point_types.h>

#include "impl/region_growing.hpp"
#include "impl/region_growing_rgb.hpp"

#include <pcl/filters/passthrough.h>

#include <pcl/ModelCoefficients.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/features/normal_3d.h>
#include <pcl/kdtree/kdtree.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/segmentation/extract_clusters.h>

#include <pcl/filters/passthrough.h>

#include <pcl/segmentation/region_growing_rgb.h>

using pcl_ptr = pcl::PointCloud<pcl::PointXYZRGBA>::Ptr;

//-----------------------------------------------------------------------------
// TOOLS
//-----------------------------------------------------------------------------

#include "er-logging.h"

//-----------------------------------------------------------------------------
// FARM PIPELINE
//-----------------------------------------------------------------------------

#include "process_3d.h"

namespace er {
	// Lockable object that contains our raw point cloud
	class frame_data : public boost::basic_lockable_adapter<boost::mutex>
	{
	public:
		bool initialized;
		uint32_t time_t;

		boost::mutex mtx;

		volatile uint8_t idx;
		volatile bool invalidate;

		pcl_ptr cloud;

		frame_data();

		void invalidate_cloud(pcl_ptr cloud_);
	};

	// We have filters on the system that accept point clouds and outputs
	// results.
	//
	// The filters create a tree of dependencies and can be multithreaded.
	class filter
	{
	public:
		filter();
		~filter();

		void input(pcl_ptr cloud);
		pcl_ptr output();
	};

    class pipeline
    {
    public:
        pipeline();
        ~pipeline();

        // Preconfigures a folder where the information for the video is stored.
        // Preprocess all the information on that folder and caches the frame data
        // in memory.
        void initialize_folder(std::string folder_path);

		void process_frame(pcl_ptr cloud);
    };
}

#endif