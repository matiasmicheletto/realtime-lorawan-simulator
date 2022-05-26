const euclideanDistance = (p1, p2) => Math.sqrt(Math.pow(p2.x - p1.x, 2) + Math.pow(p2.y - p1.y, 2));

export const getClosest = (target, positions) => {
    // Returns the closest element to target from positions
    const closest = {
        dist: Number.MAX_SAFE_INTEGER,
        idx: -1
    };
    for(let i = 0; i < positions.length; i++) {
        const dist = euclideanDistance(target, positions[i]);
        if(dist < closest.dist) {
            closest.dist = dist;            
            closest.idx = i;
        }
    }
    return closest;
}

export const sortByClosest = (target, positions, limit) => {    
    // Returns the list of positions sorted by the closest element to target
    return positions
    .map((el, idx) => {
        return {
            dist: euclideanDistance(target, el),
            idx: idx
        };
    })
    .sort((a, b) => a.dist - b.dist)
    .filter(el => el.dist <= limit)
};