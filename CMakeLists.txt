cmake_minimum_required(VERSION 3.31)
project(opencv_playground)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenCV REQUIRED)
find_package(JPEG REQUIRED)

#add_executable(opencv_playground main.cpp)

# === DEMOS ===

#add_executable(tenengrad_focus tenengrad_focus/main.cpp)
#target_link_libraries(tenengrad_focus PRIVATE ${OpenCV_LIBS})
#
#add_executable(coin_detection coin_detection/main.cpp)
#target_link_libraries(coin_detection PRIVATE ${OpenCV_LIBS})
#
#add_executable(image_inpainting image_inpainting/main.cpp)
#target_link_libraries(image_inpainting PRIVATE ${OpenCV_LIBS})

add_executable(tenengrad_focus tenengrad_focus/main.cpp)
target_link_libraries(tenengrad_focus PRIVATE opencv::opencv JPEG::JPEG)

add_executable(coin_detection coin_detection/main.cpp)
target_link_libraries(coin_detection PRIVATE opencv::opencv JPEG::JPEG)

add_executable(image_inpainting image_inpainting/main.cpp)
target_link_libraries(image_inpainting PRIVATE opencv::opencv JPEG::JPEG)

add_executable(screen_matting screen_matting/main.cpp)
target_link_libraries(screen_matting PRIVATE opencv::opencv JPEG::JPEG)

add_executable(homography_warp homography_warp/main.cpp)
target_link_libraries(homography_warp PRIVATE opencv::opencv JPEG::JPEG)

add_executable(image_registration image_registration/main.cpp)
target_link_libraries(image_registration PRIVATE opencv::opencv JPEG::JPEG)

add_executable(find_known_objects find_known_objects/main.cpp)
target_link_libraries(find_known_objects PRIVATE opencv::opencv JPEG::JPEG)

add_executable(panorama_stitching panorama_stitching/main.cpp)
target_link_libraries(panorama_stitching PRIVATE opencv::opencv JPEG::JPEG)

add_executable(align_rgb_channels align_rgb_channels/main.cpp)
target_link_libraries(align_rgb_channels PRIVATE opencv::opencv JPEG::JPEG)
