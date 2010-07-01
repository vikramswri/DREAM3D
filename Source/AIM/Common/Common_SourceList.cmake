
set (AIM_COMMON_SRCS
    ${TO79_SOURCE_DIR}/Source/AIM/Common/AIMImage.cpp
)
set (AIM_COMMON_HDRS
    ${TO79_SOURCE_DIR}/Source/AIM/Common/AIMArray.hpp
    ${TO79_SOURCE_DIR}/Source/AIM/Common/AIMImage.h   )
IDE_SOURCE_PROPERTIES("AIM/Common" "${AIM_COMMON_HDRS}" "${AIM_COMMON_SRCS}")
