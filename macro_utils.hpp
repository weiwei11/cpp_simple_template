#ifndef MACRO_UTILS_HPP_
#define MACRO_UTILS_HPP_

#define CV_SHOW_RUN_TIME
// #ifdef CV_SHOW_RUN_TIME
//     int64 cv_time_record_start;
//     int64 cv_time_record_end;

//     #define CV_SHOW_RUN_TIME_BEGIN() \
//         cv_time_record_start = cv::getTickCount();

//     #define CV_SHOW_RUN_TIME_END(fun_name) \
//         cv_time_record_end = cv::getTickCount(); \
//         std::cout << (fun_name) << ": " << 1000 * (cv_time_record_end-cv_time_record_start) / (double)cv::getTickFrequency() << " ms" << std::endl;
// #else
//     #define CV_SHOW_RUN_TIME_BEGIN() 
//     #define CV_SHOW_RUN_TIME_END(fun_name) 
// #endif
#ifdef CV_SHOW_RUN_TIME
    int64 cv_time_record_start;
    int64 cv_time_record_end;

    #define CV_SHOW_RUN_TIME_BEGIN() \
        cv_time_record_start = cv::getTickCount();

    #define CV_SHOW_RUN_TIME_END(fun_name) \
        cv_time_record_end = cv::getTickCount(); \
        std::cout << (fun_name) << ": " << 1000 * (cv_time_record_end-cv_time_record_start) / (double)cv::getTickFrequency() << " ms" << std::endl;
#else
    #define CV_SHOW_RUN_TIME_BEGIN() 
    #define CV_SHOW_RUN_TIME_END(fun_name) 
#endif



#define CV_CONVERT_TOOL
#ifdef CV_CONVERT_TOOL
    #define CV_FORALL_MAT_TENSOR_TYPE_MAP(_) \
        _(CV_32FC1, torch::kFloat32) \
        _(CV_64FC1, torch::kFloat64) \
        _(CV_8UC1, torch::kUInt8) \
        _(CV_32SC1, torch::kInt32)

    static inline int ScalarTypeToCVMatType(torch::ScalarType scalar_type) 
    {
        #define DEFINE_CASE(cv_type, torch_type) \
        case torch_type:            \
            return cv_type;

        switch (scalar_type) 
        {
            CV_FORALL_MAT_TENSOR_TYPE_MAP(DEFINE_CASE)
            default:
            AT_ERROR(
                "Unrecognized Scalartype ",
                scalar_type,
                " (please report this error)");
        }
        #undef DEFINE_CASE
    }

    static inline torch::ScalarType CVMatTypeToScalarType(int cv_mat_type)
    {
        #define DEFINE_CASE(cv_type, torch_type) \
        case cv_type:            \
            return torch_type;

        switch (cv_mat_type) 
        {
            CV_FORALL_MAT_TENSOR_TYPE_MAP(DEFINE_CASE)
            default:
            AT_ERROR(
                "Unrecognized Scalartype ",
                cv_mat_type,
                " (please report this error)");
        }
        #undef DEFINE_CASE
    }
#endif

#define CV_FILESTORAGE_TOOL
#ifdef CV_FILESTORAGE_TOOL
    #define CV_MAT_SAVB(file_storage, cv_mat) \
        if (!file_storage.isOpened()) \
        { \
            std::cerr << "failed to open " << #file_storage << std::endl; \
        } \
        file_storage << #cv_mat << cv_mat;

    #define CV_MAT_LOAD(file_storage, cv_mat) \
        if (!file_storage.isOpened()) \
        { \
            std::cerr << "failed to open " << #file_storage << std::endl; \
        } \
        if (file_storage[#cv_mat].empty()) \
        { \
            std::cerr << #cv_mat << " is not exist!" << std::endl; \
        } \
        file_storage[#cv_mat] >> cv_mat;

    #define CV_TENSOR_SAVE(file_storage, torch_tensor) \
        if (!file_storage.isOpened()) \
        { \
            std::cerr << "failed to open " << #file_storage << std::endl; \
        } \
        file_storage << #torch_tensor << cv::Mat(1, torch_tensor.numel(), ScalarTypeToCVMatType(torch::typeMetaToScalarType(torch_tensor.dtype())), torch_tensor.to(torch::kCPU).data_ptr());

#endif

#define TEST_COMPARE_TOOL
#ifdef TEST_COMPARE_TOOL
    #define COMPARE_MAT_FROM_FILESTORAGE(cv_mat, variable_name, file_storage) \
        cv::Mat variable_name; \
        CV_MAT_LOAD(file_storage, variable_name) \
        std::cout << __FUNCTION__ << " " << __LINE__ << ": " << #cv_mat << " and " << #variable_name << " different is " << cv::norm(cv_mat.reshape(1, 1) - variable_name.reshape(1, 1)) << std::endl;

    #define COMPARE_MAT_FROM_FILESTORAGE_SIMPLE(cv_mat, file_storage) \
        COMPARE_MAT_FROM_FILESTORAGE(cv_mat, correct_##cv_mat, file_storage);
#else
    #define COMPARE_MAT_FROM_FILESTORAGE(cv_mat, variable_name, file_storage)
    #define COMPARE_MAT_FROM_FILESTORAGE_SIMPLE(cv_mat, file_storage) 
#endif

#endif // MACRO_UTILS_HPP_