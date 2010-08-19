
set (AIM_ImageFilters_SRCS
    ${IPHelper_CODE_DIR}/AIM/ImageFilters/AIMImageFilter.cpp
    ${IPHelper_CODE_DIR}/AIM/ImageFilters/AIMCropGrayScaleImage.cpp
    ${IPHelper_CODE_DIR}/AIM/ImageFilters/AIMImagePadFilter.cpp
)
set (AIM_ImageFilters_HDRS
    ${IPHelper_CODE_DIR}/AIM/ImageFilters/AIMImageFilter.h
    ${IPHelper_CODE_DIR}/AIM/ImageFilters/AIMCropGrayScaleImage.h
    ${IPHelper_CODE_DIR}/AIM/ImageFilters/AIMImagePadFilter.h
    )
IDE_SOURCE_PROPERTIES("AIM/ImageFilters" "${AIM_ImageFilters_HDRS}" "${AIM_ImageFilters_SRCS}")
