#pragma once

#include "SgDevice.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace SunGlassEngine {

    class SgDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(SgDevice& sgDevice) : sgDevice{ sgDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<SgDescriptorSetLayout> build() const;

        private:
            SgDevice& sgDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        SgDescriptorSetLayout(
            SgDevice& sgDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~SgDescriptorSetLayout();
        SgDescriptorSetLayout(const SgDescriptorSetLayout&) = delete;
        SgDescriptorSetLayout& operator=(const SgDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        SgDevice& sgDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class SgDescriptorWriter;
    };

    class SgDescriptorPool {
    public:
        class Builder {
        public:
            Builder(SgDevice& sgDevice) : sgDevice{ sgDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<SgDescriptorPool> build() const;

        private:
            SgDevice& sgDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        SgDescriptorPool(
            SgDevice& sgDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~SgDescriptorPool();
        SgDescriptorPool(const SgDescriptorPool&) = delete;
        SgDescriptorPool& operator=(const SgDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        SgDevice& sgDevice;
        VkDescriptorPool descriptorPool;

        friend class SgDescriptorWriter;
    };

    class SgDescriptorWriter {
    public:
        SgDescriptorWriter(SgDescriptorSetLayout& setLayout, SgDescriptorPool& pool);

        SgDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        SgDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        SgDescriptorSetLayout& setLayout;
        SgDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace sg