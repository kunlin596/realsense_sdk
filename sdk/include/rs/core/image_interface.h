// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2016 Intel Corporation. All Rights Reserved.

#pragma once
#include "metadata_interface.h"
#include "rs/core/ref_count_interface.h"
#include "rs/utils/release_self_base.h"
#include "types.h"

namespace rs
{   /**
     * @class frame
     * forward declaraion of rs::frame.
     */
    class frame;

    namespace core
    {
        /**
        * @class image_interface
        * @brief Image interface abstracts interactions with images.
        *
        * Due to ABI restriction the image_interface object memory is managed by the inherent ref_count_interface,
        * users are obligated to release the image memory using the release function instead of deleting the object directly.
        */
        class image_interface : public ref_count_interface
        {
        public:
            /**
            * @enum flag
            * @brief Describes the image flags.
            */
            enum flag
            {
                any = 0x0
            };

            /**
            * @brief Return the image sample information.
            *
            * @return image_info    the image sample information in the image_info structure.
            */
            virtual image_info query_info(void) const = 0;

            /**
            * @brief Get the image timestamp.
            *
            * @return double    the timestamp value, in milliseconds since the device was started.
            */
            virtual double query_time_stamp(void) const = 0;

            /**
            * @brief Get the image timestamp domain.
            *
            * Used to check if two timestamp values are comparable (generated from the same clock).
            * @return timestamp_domain    the timestamp domain value.
            */
            virtual timestamp_domain query_time_stamp_domain(void) const = 0;

            /**
            * @brief Get the image flags.
            *
            * @return image_interface::flag    the flags.
            */
            virtual flag query_flags(void) const = 0;

            /**
            * @brief Get the image data.
            *
            * @return const void *    the data.
            */
            virtual const void * query_data(void) const = 0;

            /**
            * @brief Return the image stream type.
            *
            * @return stream_type    the stream type.
            */
            virtual stream_type query_stream_type(void) const = 0;

            /**
            * @brief Return the image frame number.
            *
            * @return uint64_t    the frame number.
            */
            virtual uint64_t query_frame_number(void) const = 0;

            /**
            * @brief Return metadata of the image.
            *
            * @return metadata_interface *    the image metadata
            */
            virtual metadata_interface* query_metadata() = 0;

            /**
            * @brief Convert the current image image to a given format.
            *
            * @param[in]  format                    Destination format.
            * @param[out] converted_image           Converted image allocated internaly.
            * @return status_no_error               Successful execution.
            * @return status_param_unsupported      Convertion to this format is currently unsupported.
            * @return status_feature_unsupported    The feature is currently unsupported.
            * @return status_exec_aborted           Failed to convert.
            */
            virtual status convert_to(pixel_format format, const image_interface ** converted_image) = 0;

            /**
            * @brief Convert the current image image to a given format.
            *
            * @param[in]  rotation                  Destination rotation.
            * @param[out] converted_image           Converted image allocated internaly.
            * @return status_no_error               Successful execution.
            * @return status_param_unsupported      Convertion to this format is currently unsupported.
            * @return status_feature_unsupported    The feature is currently unsupported.
            * @return status_exec_aborted           Failed to convert.
            */
            virtual status convert_to(rotation rotation, const image_interface** converted_image) = 0;

            /**
             * @brief create_instance_from_librealsense_frame.
             *
             * sdk image implementation for a frame defined by librealsense.
             * the returned image takes ownership of the frame, thus the input frame parmeter is invalid on return.
             * @param frame                 frame object defined by librealsense (rs::frame)
             * @param[in] flags             optional flags, place holder for future options
             * @return image_interface *    image_interface object.
             */
            static image_interface * create_instance_from_librealsense_frame(rs::frame& frame,
                                                                             flag flags);

            /**
             * @struct image_data_with_data_releaser
             * @brief container to unify the image data pointer and an a data releaser.
             */
            struct image_data_with_data_releaser
            {
            public:
                image_data_with_data_releaser(const void * data, release_interface * data_releaser = nullptr): data(data), data_releaser(data_releaser) {}

                const void * data; /**< the image data pointer */
                release_interface * data_releaser; /**< a data releaser defined by the user which serves as a custom deleter for the image data.
                                                        Upon calling to the interface release function, this object should release the image data and
                                                        the data releaser memory. a null data_releaser means that the image data is managed by the user
                                                        outside of the image class. for a simple data releaser implementation which deletes the data
                                                        pointer with 'delete[]' use sdk/include/rs/utils/self_releasing_array_data_releaser.h */
            };

            /**
             * @brief create_instance_from_raw_data
             *
             * sdk image implementation from raw data, where the user provides an allocated image data and
             * an optional image deallocation method with the data_releaser_interface, if no deallocation method is provided,
             * it assumes that the user is handling memory deallocation outside of the custom image class.
             * @param[in] info                  info required to successfully traverse the image data/
             * @param[in] data_container        the image data and the data releasing handler. The releasing handler release function will be called by
             *                                  the image destructor. a null data_releaser means the user is managing the image data outside of the image instance.
             * @param[in] stream                the stream type.
             * @param[in] flags                 optional flags, place holder for future options.
             * @param[in] time_stamp            the timestamp of the image, in milliseconds since the device was started.
             * @param[in] frame_number          the number of the image, since the device was started.
             * @param[in] time_stamp_domain     the domain in which the timestamp were generated from.
             * @return image_interface *    an image instance.
             */
            static image_interface * create_instance_from_raw_data(image_info * info,
                                                                   const image_data_with_data_releaser &data_container,
                                                                   stream_type stream,
                                                                   image_interface::flag flags,
                                                                   double time_stamp,
                                                                   uint64_t frame_number,
                                                                   timestamp_domain time_stamp_domain = timestamp_domain::camera);
        protected:
            virtual ~image_interface() {}
        };
    }
}

