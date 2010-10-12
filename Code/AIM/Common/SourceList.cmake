
set (AIM_Common_SRCS
    ${PROJECT_CODE_DIR}/AIM/Common/AIMImage.cpp
)
set (AIM_Common_HDRS
    ${PROJECT_CODE_DIR}/AIM/Common/AIMArray.hpp
    ${PROJECT_CODE_DIR}/AIM/Common/AIMImage.h   )
cmp_IDE_SOURCE_PROPERTIES("AIM/Common" "${AIM_Common_HDRS}" "${AIM_Common_SRCS}" "0")
