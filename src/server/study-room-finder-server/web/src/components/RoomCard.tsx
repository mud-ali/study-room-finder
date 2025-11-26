"use client";

import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import StatBar from "@/components/StatBar";

export interface RoomData {
    name: string;
    occupancy: number;
    volume: number;
    maxOccupancy: number;
}

interface RoomCardProps {
    room: RoomData;
    isLive?: boolean;
}

export default function RoomCard({ room, isLive = false }: RoomCardProps) {
    return (
        <Card className="w-full h-3/4 my-6">
            <CardHeader>
                <CardTitle>
                    {room.name}
                    <div className="text-xs font-normal my-1">
                        {isLive ? (
                            <>
                                <span className="text-[8pt]">🔴&nbsp;</span>
                                <span>Live</span>
                            </>
                        ) : (
                            <>
                                <span className="text-[8pt]">⚪&nbsp;</span>
                                <span>Offline</span>
                            </>
                        )}
                    </div>
                </CardTitle>
            </CardHeader>
            <CardContent>
                <StatBar descriptorLabel="Occupancy" value={room.occupancy} roomName={room.name} />
                <StatBar descriptorLabel="Volume" value={room.volume} roomName={room.name} />
            </CardContent>
        </Card>
    );
}
