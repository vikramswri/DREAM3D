
set (AIM_Common_SRCS
    ${IPHelper_CODE_DIR}/AIM/Common/AIMImage.cpp
)
set (AIM_Common_HDRS
    ${IPHelper_CODE_DIR}/AIM/Common/AIMArray.hpp
    ${IPHelper_CODE_DIR}/AIM/Common/AIMImage.h   )
IDE_SOURCE_PROPERTIES("AIM/Common" "${AIM_Common_HDRS}" "${AIM_Common_SRCS}")
