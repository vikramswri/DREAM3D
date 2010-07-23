
set (AIM_ImageFilters_SRCS
    ${TO79_CODE_DIR}/AIM/ImageFilters/AIMImageFilter.cpp
    ${TO79_CODE_DIR}/AIM/ImageFilters/AIMCropGrayScaleImage.cpp
    ${TO79_CODE_DIR}/AIM/ImageFilters/AIMImagePadFilter.cpp
)
set (AIM_ImageFilters_HDRS
    ${TO79_CODE_DIR}/AIM/ImageFilters/AIMImageFilter.h
    ${TO79_CODE_DIR}/AIM/ImageFilters/AIMCropGrayScaleImage.h
    ${TO79_CODE_DIR}/AIM/ImageFilters/AIMImagePadFilter.h
    )
IDE_SOURCE_PROPERTIES("AIM/ImageFilters" "${AIM_ImageFilters_HDRS}" "${AIM_ImageFilters_SRCS}")
