"use client";

import * as React from "react";
import RoomCard from "@/components/RoomCard";
import type { RoomData } from "@/components/RoomCard";

interface RoomDashboardProps {
    serverPort: number;
    initialData?: RoomData[];
}

const REFRESH_TIME = 1000; 

export default function RoomDashboard({ serverPort, initialData = [] }: RoomDashboardProps) {
    const [rooms, setRooms] = React.useState<RoomData[]>(initialData);
    const [error, setError] = React.useState<string | null>(null);

    const fetchRooms = React.useCallback(async () => {
        try {
            const res = await fetch(`http://${window.location.host.split(":")[0]}:${serverPort}/refresh`);
            if (!res.ok) throw new Error(`Fetch failed: ${res.status}`);
            const data = await res.json();
            setRooms(data);
            setError(null);
        } catch (err) {
            console.error("Error fetching room data:", err);
            setError("Failed to fetch room data");
        }
    }, [serverPort]);

    React.useEffect(() => {
        // get initial data and then re-request on a set interval
        fetchRooms();
        const interval = setInterval(fetchRooms, REFRESH_TIME);
        return () => clearInterval(interval);
    }, [fetchRooms]);

    if (error && rooms.length === 0) {
        return <div className="text-red-500 text-center p-4">{error}</div>;
    }

    return (
        <div className="auto-rows-fr grid place-items-center gap-6 grid-cols-1 sm:grid-cols-2 md:grid-cols-3 lg:grid-cols-4 w-full max-w-6xl mx-auto my-5 px-4">
            {rooms.map((room) => (
                <RoomCard key={room.name} room={room} isLive={error === null} />
            ))}
        </div>
    );
}
