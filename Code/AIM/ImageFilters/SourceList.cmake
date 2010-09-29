
set (AIM_ImageFilters_SRCS
    ${PROJECT_CODE_DIR}/AIM/ImageFilters/AIMImageFilter.cpp
    ${PROJECT_CODE_DIR}/AIM/ImageFilters/AIMCropGrayScaleImage.cpp
    ${PROJECT_CODE_DIR}/AIM/ImageFilters/AIMImagePadFilter.cpp
)
set (AIM_ImageFilters_HDRS
    ${PROJECT_CODE_DIR}/AIM/ImageFilters/AIMImageFilter.h
    ${PROJECT_CODE_DIR}/AIM/ImageFilters/AIMCropGrayScaleImage.h
    ${PROJECT_CODE_DIR}/AIM/ImageFilters/AIMImagePadFilter.h
    )
cmp_IDE_SOURCE_PROPERTIES("AIM/ImageFilters" "${AIM_ImageFilters_HDRS}" "${AIM_ImageFilters_SRCS}" "0")
