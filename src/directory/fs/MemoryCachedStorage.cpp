/* Copyright (C) 2014-2015 Alexander Shishenko <GamePad64@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "MemoryCachedStorage.h"
#include "FSDirectory.h"

namespace librevault {

MemoryCachedStorage::MemoryCachedStorage(FSDirectory& dir) : AbstractStorage(dir), Loggable(dir, "MemoryCachedStorage") {}

bool MemoryCachedStorage::have_block(const blob& encrypted_data_hash) const {
	return cache_iteraror_map_.find(encrypted_data_hash) != cache_iteraror_map_.end();
}

std::shared_ptr<blob> MemoryCachedStorage::get_block(const blob& encrypted_data_hash) const {
	auto it = cache_iteraror_map_.find(encrypted_data_hash);
	if(it == cache_iteraror_map_.end()) {
		throw AbstractDirectory::no_such_block();
	}else{
		cache_list_.splice(cache_list_.begin(), cache_list_, it->second);
		return it->second->second;
	}
}

void MemoryCachedStorage::put_block(const blob& encrypted_data_hash, std::shared_ptr<blob> data) {
	auto it = cache_iteraror_map_.find(encrypted_data_hash);
	if(it != cache_iteraror_map_.end()) {
		cache_list_.erase(it->second);
		cache_iteraror_map_.erase(it);
	}

	cache_list_.push_front(encrypted_data_hash_data_type(encrypted_data_hash, data));
	cache_iteraror_map_[encrypted_data_hash] = cache_list_.begin();

	if(overflow()) {
		auto last = cache_list_.end();
		last--;
		cache_iteraror_map_.erase(last->first);
		cache_list_.pop_back();
	}
}

void MemoryCachedStorage::remove_block(const blob& encrypted_data_hash) {
	auto iterator_to_iterator = cache_iteraror_map_.find(encrypted_data_hash);
	cache_list_.erase(iterator_to_iterator->second);
	cache_iteraror_map_.erase(iterator_to_iterator);
}

bool MemoryCachedStorage::overflow() const {
	return cache_iteraror_map_.size() > 10; // TODO: implement other cache clearing strategy.
}

} /* namespace librevault */
